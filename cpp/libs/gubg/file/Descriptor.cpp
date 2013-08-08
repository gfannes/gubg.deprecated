#include "gubg/file/Descriptor.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "Descr::Pimpl"
#include "gubg/log/begin.hpp"
struct Descriptor::Pimpl
{
    int desc;
    Pimpl(int d):desc(d){}
    ~Pimpl()
    {
        ::close(desc);
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
    res.pimpl_.reset(new Pimpl(desc));

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
Descriptor Descriptor::listen(File f)
{
    Descriptor res;
    return res;
}

ReturnCode Descriptor::accept(Descriptor &s)
{
    MSS_BEGIN(ReturnCode);
    MSS(pimpl_);
    Pimpl &me = *pimpl_;
    s.reset();
    int desc;
    struct sockaddr_storage peer_addr;
    socklen_t addr_sz = sizeof(peer_addr);
    MSS((desc = ::accept(me.desc, (struct sockaddr *)&peer_addr, &addr_sz)) != -1, FailedToAccept);
    s.pimpl_.reset(new Pimpl(desc));
    MSS_END();
}
#include "gubg/log/end.hpp"
