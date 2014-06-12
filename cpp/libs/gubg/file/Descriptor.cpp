#include "gubg/file/Descriptor.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"

#ifdef GUBG_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif
#ifdef GUBG_MINGW
#include <Winsock2.h>
#include <WS2tcpip.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>

#include <chrono>
#include <thread>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <sstream>
using namespace gubg::file;
using namespace std;
using std::chrono::milliseconds;

#define GUBG_MODULE "Descriptor"
#include "gubg/log/begin.hpp"
namespace gubg { namespace file { 
    const char *to_hr(EventType et)
    {
        switch (et)
        {
#define L_CASE(n) case EventType::n: return #n
            L_CASE(Read);
            L_CASE(Write);
            L_CASE(Open);
            L_CASE(CloseRead);
            L_CASE(CloseWrite);
#undef L_CASE
        }
        return "Unknown EventType";
    }
} } 

namespace 
{

#ifdef GUBG_MINGW
    struct InitializeWSA
    {
        bool ok = false;

        InitializeWSA()
        {
#define WSA_MAKEWORD(x, y) ((y) * 256 + (x))
            WORD version_required = WSA_MAKEWORD(1, 1);
            WSADATA wsaData;
            if (::WSAStartup(version_required, &wsaData) != 0)
                return;

            if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
                return;

            ok = true;
        }
        ~InitializeWSA()
        {
            if (ok)
                ::WSACleanup();
        }
    };
    InitializeWSA initializeWSA;
#endif

    enum class Role { Normal, Acceptor };
    const char *to_hr(Role r)
    {
        switch (r)
        {
            case Role::Normal: return "Normal";
            case Role::Acceptor: return "Acceptor";
        }
        return "Unknown role";
    }

    enum class Type { Socket, File, Stream };
    const char *to_hr(Type t)
    {
        switch (t)
        {
            case Type::Socket: return "Socket";
            case Type::File: return "File";
            case Type::Stream: return "Stream";
        }
        return "Unknown type";
    }

    void translate_(int &flags, AccessMode am)
    {
        switch (am)
        {
#define L_CASE(from, to) case AccessMode::from: flags = to; break
            L_CASE(Read, O_RDONLY);
            L_CASE(Write, O_WRONLY);
            L_CASE(ReadWrite, O_RDWR);
#undef L_CASE
        }
    }
    void translate_(EventType &et, Role role, AccessMode am)
    {
        switch (role)
        {
            case Role::Normal:   et = (am == AccessMode::Read ? EventType::Read : EventType::Write); break;
            case Role::Acceptor: et = EventType::Open; break;
        }
    }
}

struct Descriptor::Pimpl: public enable_shared_from_this<Pimpl>
{
    typedef shared_ptr<Pimpl> Ptr;

    int desc;
    File fn;
    const Role role;
    const Type type;
    int accessMode;
    Pimpl *peer = 0;

    Pimpl(Role r, Type t): desc(InvalidDesc), role(r), type(t), accessMode(0){}
    ~Pimpl()
    {
        clear();
    }
    void clear()
    {
        S();
        if (desc != InvalidDesc)
        {
#ifdef GUBG_LINUX
            ::close(desc);
#endif
#ifdef GUBG_MINGW
            if (type == Type::Socket)
            {
                L("Closing socket desc");
                ::closesocket(desc);
            }
#endif
            desc = InvalidDesc;
        }
        if (peer)
        {
            peer->peer = 0;
            peer = 0;
        }
    }
    bool operator<(const Pimpl &rhs) const
    {
        //We first compare the file descriptors, this is handy for the ::select() systemcall, this needs the largest descriptor
        //If they are equal, we will compare the this pointers
        return make_pair(desc, this) < make_pair(rhs.desc, &rhs);
    }
    void stream(ostream &os) const
    {
        os << STREAM(desc, fn, to_hr(role), to_hr(type));
    }
    ReturnCode selectNonBlock(EventType &et, AccessMode am)
    {
        MSS_BEGIN(ReturnCode, STREAM(to_hr(role), to_hr(type), desc));
        switch (type)
        {
            case Type::File:
                switch (role)
                {
                    case Role::Acceptor:
                        MSS_Q(!peer);
                        MSS_Q(exists(fn));
                        L("File " << fn << " exists and has no peer yet");
                        et = EventType::Open;
                        break;
                }
                break;
            default: MSS_L(NotSupported); break;
        }
        MSS_END();
    }
    bool valid() const
    {
        if (role == Role::Acceptor)
        {
            switch (type)
            {
                case Type::File: return !fn.empty(); break;
            }
        }
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
        MSS(!peer, AlreadyAcceptedFile);
        int d;
        MSS((d = ::open(fn.name().c_str(), accessMode)) != -1, FailedToOpen);
        pp.reset(new Pimpl(Role::Normal, Type::File));
        pp->fn = fn;
        pp->desc = d;
        pp->peer = this;
        peer = pp.get();
        MSS_END();
    }
    ReturnCode acceptStdin(Ptr &pp)
    {
        MSS_BEGIN(ReturnCode);
        MSS(!peer, AlreadyAcceptedFile);
        pp.reset(new Pimpl(Role::Normal, Type::File));
        pp->fn = fn;
        pp->desc = STDIN_FILENO;
        pp->peer = this;
        peer = pp.get();
        MSS_END();
    }
    ReturnCode read(string &buffer)
    {
        MSS_BEGIN(ReturnCode);

        MSS(!buffer.empty());
        MSS(role == Role::Normal);
        MSS(desc != InvalidDesc);

        ssize_t s = 0;
        switch (type)
        {
            case Type::Socket:
                s = ::recv(desc, &buffer[0], buffer.size(), 0);
                break;
            default:
#ifdef GUBG_LINUX
                //Transparent reading from files is only supported in linux
                s = ::read(desc, &buffer[0], buffer.size());
#endif
                break;
        }
        L("I read " << s << " bytes");
        MSS(s >= 0, FailedToRead);
        MSS_Q(s > 0, PeerClosedConnection);
        MSS(s <= buffer.size());
        buffer.resize(s);

        L("I read " << buffer.size() << " bytes");

        MSS_END();
    }
    ReturnCode write(size_t &written, const string &buffer)
    {
        MSS_BEGIN(ReturnCode);

        MSS(!buffer.empty());
        MSS(role == Role::Normal);
        MSS(desc != InvalidDesc);

#ifdef GUBG_LINUX
        const auto s = ::write(desc, &buffer[0], buffer.size());
#endif
#ifdef GUBG_MINGW
        MSS(type == Type::Socket);
        const auto s = ::send(desc, &buffer[0], buffer.size(), 0);
#endif
        L(STREAM(desc, s));
        MSS(s != -1, FailedToWrite);
        MSS(s <= buffer.size());
        written = s;
        L("I wrote " << written << " bytes");

        MSS_END();
    }
    ReturnCode shutdown(AccessMode am)
    {
        MSS_BEGIN(ReturnCode);

        MSS(role == Role::Normal);
        MSS(desc != InvalidDesc);

#ifdef GUBG_LINUX
        switch (am)
        {
            case AccessMode::Read:
                ::shutdown(desc, SHUT_RD);
                break;
            case AccessMode::Write:
                ::shutdown(desc, SHUT_WR);
                break;
            case AccessMode::ReadWrite:
                ::shutdown(desc, SHUT_RDWR);
                break;
        }
#endif
#ifdef GUBG_MINGW
        switch (am)
        {
            case AccessMode::Read:
                ::shutdown(desc, SD_RECEIVE);
                break;
            case AccessMode::Write:
                ::shutdown(desc, SD_SEND);
                break;
            case AccessMode::ReadWrite:
                ::shutdown(desc, SD_BOTH);
                break;
        }
#endif

        MSS_END();
    }
    ReturnCode setBaudRate(int rate)
    {
        MSS_BEGIN(ReturnCode);
        MSS(role == Role::Normal);
        MSS(type == Type::File);
        ostringstream cmd;
        cmd << "stty -F " << fn.name() << " " << rate << " raw";
        MSS(::system(cmd.str().c_str()) == 0);
        MSS_END();
    }
};

size_t Descriptor::id() const
{
    return (size_t)pimpl_.get();
}
Descriptor Descriptor::listen(unsigned short port, const string &ip)
{
    S();

    struct addrinfo hints;
    struct addrinfo *servinfo = 0;
    memset(&hints, 0, sizeof(hints));
    //hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

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
Descriptor Descriptor::std_in()
{
    Descriptor res;
    res.pimpl_.reset(new Pimpl(Role::Normal, Type::Stream));
    res.pimpl_->desc = 0;
    translate_(res.pimpl_->accessMode, AccessMode::Read);
    return res;
}
Descriptor Descriptor::connect(const string &ip, unsigned short port)
{
    S();

    struct addrinfo hints;
    struct addrinfo *servinfo = 0;
    memset(&hints, 0, sizeof(hints));
    //hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    {
        ostringstream oss; oss << port;
        const auto rc = ::getaddrinfo((ip.empty() ? 0 : ip.c_str()), oss.str().c_str(), &hints, &servinfo);
        //const auto rc = ::getaddrinfo((ip.empty() ? 0 : ip.c_str()), 0, &hints, &servinfo);
        if (rc != 0)
        {
            L("Failed to get address info " << rc << " " << gai_strerror(rc));
            return Descriptor();
        }
        assert(servinfo != 0);
    }

    const auto desc = ::socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (desc == -1)
    {
        L("Failed to get a socket");
        return Descriptor();
    }
    L("Got descriptor " << desc);

    if (::connect(desc, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        L("Failed to connect");
        return Descriptor();
    }

    Descriptor res;
    res.pimpl_.reset(new Pimpl(Role::Normal, Type::Socket));
    res.pimpl_->desc = desc;

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
ReturnCode Descriptor::read(string &buffer)
{
    MSS_BEGIN(ReturnCode);
    MSS(pimpl_);
    MSS(pimpl_->read(buffer));
    MSS_END();
}
ReturnCode Descriptor::write(size_t &written, const string &buffer)
{
    MSS_BEGIN(ReturnCode);
    MSS(pimpl_);
    MSS(pimpl_->write(written, buffer));
    MSS_END();
}
ReturnCode Descriptor::shutdown(AccessMode am)
{
    MSS_BEGIN(ReturnCode);
    MSS(pimpl_);
    MSS(pimpl_->shutdown(am));
    MSS_END();
}
ReturnCode Descriptor::setBaudRate(int rate)
{
    MSS_BEGIN(ReturnCode);
    MSS(pimpl_);
    MSS(pimpl_->setBaudRate(rate));
    MSS_END();
}

bool Descriptor::valid() const
{
    if (!pimpl_)
        return false;
    return pimpl_->valid();
}

void Descriptor::stream(ostream &os) const
{
    if (!pimpl_)
    {
        os << "Descriptor==nil";
        return;
    }
    pimpl_->stream(os);
}
bool Descriptor::operator<(const Descriptor &rhs) const
{
    return pimpl_ < rhs.pimpl_;
}
bool Descriptor::operator==(const Descriptor &rhs) const
{
    return pimpl_ == rhs.pimpl_;
}

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
    L(STREAM(read_set_.size(), write_set_.size()));
    for (auto d: read_set_)
        L(Descriptor(d));
    for (auto d: write_set_)
        L(Descriptor(d));
    MSS_END();
}
ReturnCode Select::erase(Descriptor desc, AccessMode accessMode)
{
    MSS_BEGIN(ReturnCode);
    assert(invariants_());
    switch (accessMode)
    {
        case AccessMode::Read:
            read_set_.erase(desc.pimpl_);
            break;
        case AccessMode::Write:
            write_set_.erase(desc.pimpl_);
            break;
        case AccessMode::ReadWrite:
            read_set_.erase(desc.pimpl_);
            write_set_.erase(desc.pimpl_);
            break;
    }
    assert(invariants_());
    L(STREAM(read_set_.size(), write_set_.size()));
    for (auto d: read_set_)
        L(Descriptor(d));
    for (auto d: write_set_)
        L(Descriptor(d));
    MSS_END();
}
namespace 
{
    template <typename Set>
        void translate_(fd_set &fds, const Set &s)
        {
            FD_ZERO(&fds);
            for (auto p: s)
            {
                auto desc = p->desc;
                if (desc == Descriptor::InvalidDesc)
                    continue;
                FD_SET(desc, &fds);
            }
        }
    template <typename Receiver, typename DSet, typename Timeout>
        ReturnCode select_(Receiver &receiver, int maxDesc, const DSet &rds, const DSet &wds, const Timeout *timeout)
        {
            MSS_BEGIN(ReturnCode);

            fd_set rs; translate_(rs, rds);
            fd_set ws; translate_(ws, wds);

            struct timeval *pto = 0;
            struct timeval to;
            if (timeout)
            {
                L("Setting timeval");
                to.tv_sec = timeout->count()/1000;
                to.tv_usec = 1000*(timeout->count()%1000);
                pto = &to;
            }

            MSS(maxDesc != Descriptor::InvalidDesc, InvalidMaxDesc);

            {
                S();L("Waiting for ::select() to return " << STREAM(maxDesc));
                const auto ret = ::select(maxDesc+1, &rs, &ws, 0, pto);
                L("::select() returned " << ret);
                MSS(ret != -1, FailedToSelect);
                L("::select() returned OK");
            }

            auto lRds = rds;
            auto lWds = wds;
            for (auto p: lRds)
            {
                L("Checking for a read event: " << Descriptor(p));
                auto desc = p->desc;
                if (desc == Descriptor::InvalidDesc)
                    continue;
                if (FD_ISSET(desc, &rs))
                {
                    EventType et; translate_(et, p->role, AccessMode::Read);
                    receiver.select_ready(Descriptor(p), et);
                }
            }
            for (auto p: lWds)
            {
                L("Checking for a write event " << Descriptor(p));
                auto desc = p->desc;
                if (desc == Descriptor::InvalidDesc)
                    continue;
                if (FD_ISSET(desc, &ws))
                {
                    EventType et; translate_(et, p->role, AccessMode::Write);
                    receiver.select_ready(Descriptor(p), et);
                }
            }
            MSS_END();
        }
}
ReturnCode Select::process(const milliseconds *timeout)
{
    milliseconds lTimeout;
    //Make sure we are using milliseconds
    if (timeout)
        lTimeout = *timeout;
    return process_(timeout ? &lTimeout : 0);
}
namespace 
{
    struct Compare
    {
        template <typename P>
            bool operator()(const P &lhs, const P &rhs) const { return *lhs < *rhs; }
    };
    template <typename Set>
        void computeMinMaxDesc_(int &minDesc, int &maxDesc, const Set &r, const Set &w)
        {
            S();
            minDesc = maxDesc = Descriptor::InvalidDesc;
            gubg::OnlyOnce setMinDesc;
            {
                if (!r.empty())
                {
                    maxDesc = max(maxDesc, (*max_element(r.begin(), r.end(), Compare()))->desc);
                    auto rMin = (*min_element(r.begin(), r.end(), Compare()))->desc;
                    if (setMinDesc())
                        minDesc = rMin;
                    else
                        minDesc = min(minDesc, rMin);
                }
                if (!w.empty())
                {
                    maxDesc = max(maxDesc, (*max_element(w.begin(), w.end(), Compare()))->desc);
                    auto wMin = (*min_element(w.begin(), w.end(), Compare()))->desc;
                    if (setMinDesc())
                        minDesc = wMin;
                    else
                        minDesc = min(minDesc, wMin);
                }
            }
            LL(minDesc << " " << maxDesc);
        }
}
ReturnCode Select::process_(milliseconds *timeout)
{
    MSS_BEGIN(ReturnCode);
    L(STREAM(read_set_.size(), write_set_.size()));
    for (auto d: read_set_)
        L(Descriptor(d));

    assert(invariants_());

    int minDesc, maxDesc; computeMinMaxDesc_(minDesc, maxDesc, read_set_, write_set_);
    L(STREAM(minDesc, maxDesc));

    if (minDesc != Descriptor::InvalidDesc)
    {
        S();L("All descriptors are selectable");
        //We can use normal ::select()
        if (maxDesc != Descriptor::InvalidDesc)
            MSS(select_(*this, maxDesc, read_set_, write_set_, timeout));
    }
    else
    {
        S();L("Not all descriptors are selectable");
        //We have to pauze ::select() from time to time to give non-select()-able non-blocking select-lookalikes a chance
        auto timeStep = milliseconds(500);
        if (timeout and *timeout < timeStep)
            timeStep = *timeout;
        L(STREAM(timeout->count(), timeStep.count()));
        while (!timeout || (*timeout > milliseconds(0) and *timeout >= timeStep))
        {
            S();L("Processing " << timeStep.count() << " ms");
            //Do non-blocking select on those that cannot be selected
            auto lReadSet = read_set_;
            for (auto p: lReadSet)
            {
                if (p->desc != Descriptor::InvalidDesc)
                    //This thing has a descriptor, so we can use real select on it; in linux, you can select on everything!
                    continue;
                EventType et;
                switch (auto rc = p->selectNonBlock(et, AccessMode::Read))
                {
                    case ReturnCode::OK:
                        L("Event detected: " << STREAM(to_hr(et)));
                        select_ready(Descriptor(p), et);
                        break;
                }
            }
            auto lWriteSet = write_set_;
            for (auto p: lWriteSet)
            {
                if (p->desc != Descriptor::InvalidDesc)
                    //This thing has a descriptor, so we can use real select on it; in linux, you can select on everything!
                    continue;
                EventType et;
                switch (auto rc = p->selectNonBlock(et, AccessMode::Write))
                {
                    case ReturnCode::OK:
                        L("Event detected: " << STREAM(to_hr(et)));
                        select_ready(Descriptor(p), et);
                        break;
                }
            }

            //Refresh the descriptor range, things might have change already
            computeMinMaxDesc_(minDesc, maxDesc, read_set_, write_set_);
            L(STREAM(minDesc, maxDesc));
            if (maxDesc != Descriptor::InvalidDesc)
                MSS(select_(*this, maxDesc, read_set_, write_set_, &timeStep));
            else
                this_thread::sleep_for(timeStep);
            if (timeout)
                *timeout -= timeStep;
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
