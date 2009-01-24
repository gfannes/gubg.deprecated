#ifndef socket_hpp
#define socket_hpp

#ifdef MINGW32
#include <winsock.h>
#else
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

// A singleton class that ensures WSAStartup() is called before socket functions are accessed
class Socket
{
public:
    static Socket &instance(){return sInstance;}
    bool valid(){return sInstance.mValid;}

protected:
    static Socket sInstance;

private:
    Socket();
    virtual ~Socket();

    bool mValid;
};

extern Socket &gSocket;

#endif
