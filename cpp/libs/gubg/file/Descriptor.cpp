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
struct Descriptor::Pimpl
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

int Descriptor::desc() const
{
    if (!pimpl_)
        return InvalidDesc;
    return pimpl_->desc;
}

bool Descriptor::operator<(const Descriptor &rhs) const
{
    //We first compare the file descriptors, this is handy for the ::select() systemcall, this needs the largest descriptor
    //If they are equal, we will compare the pimpl_ pointer itself
    return make_pair(desc(), pimpl_.get()) < make_pair(rhs.desc(), rhs.pimpl_.get());
}
#include "gubg/log/end.hpp"

#define GUBG_MODULE "Select"
#include "gubg/log/begin.hpp"
ReturnCode Select::add(Descriptor desc, AccessMode accessMode)
{
    MSS_BEGIN(ReturnCode);
    MSS((bool)desc.pimpl_);
    MSS(desc.pimpl_->desc != Descriptor::InvalidDesc);
    switch (accessMode)
    {
        case AccessMode::Read:
            read_set_.insert(desc);
            break;
        case AccessMode::Write:
            write_set_.insert(desc);
            break;
        case AccessMode::ReadWrite:
            read_set_.insert(desc);
            write_set_.insert(desc);
            break;
    }
    MSS_END();
}
namespace 
{
    ReturnCode translate_(fd_set &fds, const set<Descriptor> &s)
    {
        MSS_BEGIN(ReturnCode);
        FD_ZERO(&fds);
        for (auto d: s)
        {
            FD_SET(d.desc(), &fds);
        }
        MSS_END();
    }
}
ReturnCode Select::operator()(std::chrono::milliseconds timeout)
{
    MSS_BEGIN(ReturnCode);
    fd_set read_fds, write_fds;
    MSS(translate_(read_fds, read_set_));
    MSS(translate_(write_fds, write_set_));
    int maxDesc = Descriptor::InvalidDesc;
    if (!read_set_.empty())
        maxDesc = max(maxDesc, read_set_.rbegin()->pimpl_->desc);
    if (!write_set_.empty())
        maxDesc = max(maxDesc, write_set_.rbegin()->pimpl_->desc);
    struct timeval *pto = 0;
    struct timeval to;
    if (timeout.count() > 0)
    {
        to.tv_sec = timeout.count()/1000;
        to.tv_usec = 1000*(timeout.count()%1000);
        pto = &to;
    }
    MSS(::select(maxDesc+1, &read_fds, &write_fds, 0, pto) != -1, FailedToSelect);
    MSS_END();
}
#include "gubg/log/end.hpp"
