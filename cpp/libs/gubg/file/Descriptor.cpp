#include "gubg/file/Descriptor.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <chrono>
using namespace gubg::file;
using namespace std;

namespace 
{
    enum class Role { Normal, Acceptor };
    enum class Type { Socket, File };

    void translate_(int &flags, AccessMode am)
    {
        switch (am)
        {
#define L_CASE(from, to) case AccessMode::from: flags = to; break
            L_CASE(Read, O_RDONLY);
            L_CASE(Write, O_WRONLY);
            L_CASE(ReadWrite, O_RDWR);
        }
    }
}

#define GUBG_MODULE_ "Descr::Pimpl"
#include "gubg/log/begin.hpp"
struct Descriptor::Pimpl: public enable_shared_from_this<Pimpl>
{
    typedef shared_ptr<Pimpl> Ptr;

    int desc;
    File fn;
    const Role role;
    const Type type;
    int accessMode;

    Pimpl(Role r, Type t): desc(InvalidDesc), role(r), type(t), accessMode(0){}
    ~Pimpl()
    {
        if (desc != InvalidDesc)
            ::close(desc);
    }
    bool operator<(const Pimpl &rhs) const
    {
        //We first compare the file descriptors, this is handy for the ::select() systemcall, this needs the largest descriptor
        //If they are equal, we will compare the this pointers
        return make_pair(desc, this) < make_pair(rhs.desc, &rhs);
    }
    void stream(ostream &os) const
    {
        os << STREAM(desc, fn);
    }
    ReturnCode selectNonBlock(Select &s, AccessMode am)
    {
        MSS_BEGIN(ReturnCode);
        if (role == Role::Acceptor && type == Type::File)
        {
        }
        else
            MSS_L(NotSupported);
        MSS_END();
    }
    bool valid() const
    {
        if (role == Role::Acceptor && type == Type::File)
            return !fn.empty();
        return desc != InvalidDesc;
    }
    ReturnCode acceptSocket(Ptr &pp)
    {
        MSS_BEGIN(ReturnCode);
        int d;
        struct sockaddr_storage peer_addr;
        socklen_t addr_sz = sizeof(peer_addr);
        MSS((d = ::accept(desc, (struct sockaddr *)&peer_addr, &addr_sz)) != -1, FailedToAccept);
        pp.reset(new Pimpl(Role::Normal, Type::Socket));
        pp->desc = d;
        MSS_END();
    }
    ReturnCode acceptFile(Ptr &pp)
    {
        MSS_BEGIN(ReturnCode);
        int d;
        MSS((d = ::open(fn.name().c_str(), accessMode)) != -1, FailedToOpen);
        pp.reset(new Pimpl(Role::Normal, Type::File));
        pp->fn = fn;
        pp->desc = d;
        MSS_END();
    }
};
#include "gubg/log/end.hpp"

#define GUBG_MODULE_ "Descriptor"
#include "gubg/log/begin.hpp"
Descriptor Descriptor::listen(unsigned short port, const std::string &ip)
{
    S();

    struct addrinfo hints;
    struct addrinfo *servinfo = 0;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    ostringstream oss;
    oss << port;

    if (::getaddrinfo((ip.empty() ? 0 : ip.c_str()), oss.str().c_str(), &hints, &servinfo) != 0)
    {
        L("Failed to get address info");
        return Descriptor();
    }
    assert(servinfo != 0);

    const auto desc = ::socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (desc == -1)
    {
        L("Failed to get a socket");
        return Descriptor();
    }
    Descriptor res;
    res.pimpl_.reset(new Pimpl(Role::Acceptor, Type::Socket));
    res.pimpl_->desc = desc;

    if (::bind(desc, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
    {
        L("Failed to bind");
        return Descriptor();
    }

    if (::listen(desc, 10) != 0)
    {
        L("Failed to listen");
        return Descriptor();
    }

    ::freeaddrinfo(servinfo);
    return res;
}
Descriptor Descriptor::listen(File f, AccessMode accessMode)
{
    Descriptor res;
    res.pimpl_.reset(new Pimpl(Role::Acceptor, Type::File));
    res.pimpl_->fn = f;
    translate_(res.pimpl_->accessMode, accessMode);
    return res;
}

ReturnCode Descriptor::accept(Descriptor &desc)
{
    MSS_BEGIN(ReturnCode);

    MSS(pimpl_);
    Pimpl &me = *pimpl_;

    MSS(me.role == Role::Acceptor);

    Pimpl::Ptr pp;
    switch (me.type)
    {
        case Type::Socket: MSS(me.acceptSocket(pp)); break;
        case Type::File:   MSS(me.acceptFile(pp)); break;
        default:           MSS_L(UnknownType); break;
    }
    desc.pimpl_ = pp;

    MSS_END();
}

bool Descriptor::valid() const
{
    if (!pimpl_)
        return false;
    return pimpl_->valid();
}

void Descriptor::stream(std::ostream &os) const
{
    if (!pimpl_)
    {
        os << "Descriptor==nil";
        return;
    }
    pimpl_->stream(os);
}
#include "gubg/log/end.hpp"

#define GUBG_MODULE_ "Select"
#include "gubg/log/begin.hpp"
ReturnCode Select::add(Descriptor desc, AccessMode accessMode)
{
    MSS_BEGIN(ReturnCode);
    assert(invariants_());
    MSS(desc.valid());
    switch (accessMode)
    {
        case AccessMode::Read:
            read_set_.insert(desc.pimpl_);
            break;
        case AccessMode::Write:
            write_set_.insert(desc.pimpl_);
            break;
        case AccessMode::ReadWrite:
            read_set_.insert(desc.pimpl_);
            write_set_.insert(desc.pimpl_);
            break;
    }
    assert(invariants_());
    MSS_END();
}
namespace 
{
    template <typename Set>
        ReturnCode translate_(fd_set &fds, bool &allSelectable, const Set &s)
        {
            MSS_BEGIN(ReturnCode);
            FD_ZERO(&fds);
            for (auto p: s)
            {
                auto desc = p->desc;
                if (desc == Descriptor::InvalidDesc)
                {
                    allSelectable = false;
                    continue;
                }
                FD_SET(desc, &fds);
            }
            MSS_END();
        }
    template <typename Receiver, typename DSet>
        ReturnCode select_(Receiver &receiver, int maxDesc, fd_set &rs, fd_set &ws, const DSet &rds, const DSet &wds, std::chrono::milliseconds timeout)
        {
            MSS_BEGIN(ReturnCode);

            struct timeval *pto = 0;
            struct timeval to;
            if (timeout.count() > 0)
            {
                to.tv_sec = timeout.count()/1000;
                to.tv_usec = 1000*(timeout.count()%1000);
                pto = &to;
            }

            if (maxDesc == Descriptor::InvalidDesc)
                L("Skipping select, no selectable descriptors found");
            else
            {
                S();L("Waiting for ::select() to return " << STREAM(maxDesc));
                MSS(::select(maxDesc+1, &rs, &ws, 0, pto) != -1, FailedToSelect);
                L("::select() returned OK");
            }

            for (auto p: rds)
            {
                auto desc = p->desc;
                if (desc == Descriptor::InvalidDesc)
                    continue;
                if (FD_ISSET(desc, &rs))
                    receiver.select_readyToRead(Descriptor(p));
            }
            for (auto p: wds)
            {
                auto desc = p->desc;
                if (desc == Descriptor::InvalidDesc)
                    continue;
                if (FD_ISSET(desc, &ws))
                    receiver.select_readyToWrite(Descriptor(p));
            }
            MSS_END();
        }
}
ReturnCode Select::operator()(std::chrono::milliseconds timeout)
{
    MSS_BEGIN(ReturnCode);

    assert(invariants_());

    bool allSelectable = true;

    fd_set read_fds, write_fds;
    {
        MSS(translate_(read_fds, allSelectable, read_set_));
        MSS(translate_(write_fds, allSelectable, write_set_));
    }

    int maxDesc = Descriptor::InvalidDesc;
    {
        if (!read_set_.empty())
            maxDesc = max(maxDesc, (*read_set_.rbegin())->desc);
        if (!write_set_.empty())
            maxDesc = max(maxDesc, (*write_set_.rbegin())->desc);
    }

    if (allSelectable)
    {
        //We can use normal ::select()
        if (maxDesc != Descriptor::InvalidDesc)
            MSS(select_(*this, maxDesc, read_fds, write_fds, read_set_, write_set_, timeout));
    }
    else
    {
        //We have to pauze ::select() from time to time to give non-select()-able non-blocking select-lookalikes a chance
        const auto timeStep = std::chrono::milliseconds(500);
        while (timeout > timeStep)
        {
            //Do non-blocking select on those that cannot be selected
            for (auto p: read_set_)
            {
                if (p->desc == Descriptor::InvalidDesc)
                    continue;
                switch (auto rc = p->selectNonBlock(*this, AccessMode::Read))
                {
                    case ReturnCode::OK:
                        L("Read selectNB");
                        break;
                    default:
                        break;
                }
            }
            for (auto p: write_set_)
            {
                if (p->desc == Descriptor::InvalidDesc)
                    continue;
                switch (auto rc = p->selectNonBlock(*this, AccessMode::Write))
                {
                    case ReturnCode::OK:
                        L("Write selectNB");
                        break;
                    default:
                        break;
                }
            }
            if (maxDesc != Descriptor::InvalidDesc)
                MSS(select_(*this, maxDesc, read_fds, write_fds, read_set_, write_set_, timeStep));
            timeout -= timeStep;
        }
    }

    assert(invariants_());

    MSS_END();
}
bool Select::invariants_() const
{
    for (auto p: read_set_)
    {
        if (!p)
            return false;
    }
    for (auto p: write_set_)
    {
        if (!p)
            return false;
    }
    return true;
}
#include "gubg/log/end.hpp"
