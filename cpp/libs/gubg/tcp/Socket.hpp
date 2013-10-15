#ifndef HEADER_gubg_tcp_Socket_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tcp_Socket_hpp_ALREADY_INCLUDED

//This module wraps the POSIX socket API into a pimpl-based object oriented API
//Extra checks are included to make sure the intended sequence of calls is followed (e.g., ::socket(), ::bind(), ::listen())

#include "gubg/tcp/Codes.hpp"
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <set>

#define GUBG_MODULE "Socket"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace tcp
    {
        enum class Role {Client, Server};

        template <typename Receiver, Role RoleT>
            class Socket_crtp
            {
                private:
                    enum State {NeedSocketDescriptor, NeedsBinding, NeedsListening, Accepting, NeedsConnect};
                    enum {InvalidFID = -1};

                public:
                    Socket_crtp():
                        fid_(InvalidFID),
                        state_(NeedSocketDescriptor){}
                    ~Socket_crtp()
                    {
                        if (fid_ != InvalidFID)
                            ::close(fid_);
                    }

                    int socket_serverPort() const {return -1;}

                    ReturnCode process()
                    {
                        MSS_BEGIN(ReturnCode, process);
                        switch (state_)
                        {
                            case NeedSocketDescriptor:
                                MSS(fid_ == InvalidFID, UnexpectedValidSocketDescriptor);
                                fid_ = ::socket(PF_INET, SOCK_STREAM, 0);
                                MSS(fid_ != InvalidFID, CouldNotGetSocketDescriptor);
                                ::fcntl(fid_, F_SETFL, O_NONBLOCK);
                                switch (RoleT)
                                {
                                    case Role::Server: changeState_(NeedsBinding); break;
                                    case Role::Client: changeState_(NeedsConnect); break;
                                    default: MSS_L(UnexpectedRole); break;
                                }
                                break;
                            case NeedsBinding:
                                {
                                    const int port = receiver_().socket_serverPort();
                                    MSS(port > 0, PortTooLow);
                                    MSS(port < 65536, PortTooHigh);
                                    std::ostringstream oss;
                                    oss << port;
                                    struct addrinfo hints, *res;
                                    ::memset(&hints, 0, sizeof(hints));
                                    hints.ai_family = AF_UNSPEC;
                                    hints.ai_socktype = SOCK_STREAM;
                                    hints.ai_flags = AI_PASSIVE;
                                    ::getaddrinfo(0, oss.str().c_str(), &hints, &res);
                                    MSS(::bind(fid_, res->ai_addr, res->ai_addrlen) != -1, CouldNotBind);
                                }
                                changeState_(NeedsListening);
                                break;
                            case NeedsListening:
                                {
                                    const int MaxNrWaitingConnections = 20;
                                    MSS(::listen(fid_, MaxNrWaitingConnections) != -1, CouldNotListen);
                                }
                                changeState_(Accepting);
                                break;
                            case Accepting:
                                if (!clients_.empty())
                                {
                                    Clients closedConnections;
                                    const size_t BufferSize = 256;
                                    char buffer[BufferSize];
                                    for (auto client: clients_)
                                    {
                                        size_t rlen = ::recv(client, buffer, BufferSize, 0);
                                        if (rlen == -1)
                                        {
                                            MSS(errno == EWOULDBLOCK, ReceiveFailure);
                                        }
                                        else if (rlen == 0)
                                        {
                                            //Connection was closed
                                            closedConnections.insert(client);
                                        }
                                        else
                                        {
                                            receiver_().socket_ReceivedData(std::string(buffer, rlen));
                                        }
                                    }
                                    for (auto client: closedConnections)
                                    {
                                        receiver_().socket_closeClient(client);
                                        ::close(client);
                                        clients_.erase(client);
                                    }
                                }
                                {
                                    struct sockaddr_storage peer;
                                    socklen_t peerSize = sizeof(peer);
                                    int peerFid = ::accept(fid_, (struct sockaddr *)&peer, &peerSize);
                                    if (peerFid == -1)
                                    {
                                        MSS(errno == EWOULDBLOCK, AcceptFailed);
                                        MSS_END();
                                    }
                                    ::fcntl(peerFid, F_SETFL, O_NONBLOCK);
                                    clients_.insert(peerFid);
                                    receiver_().socket_newClient(peerFid);
                                }
                                break;
                        }
                        MSS_END();
                    }

                private:
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                    void changeState_(State newState)
                    {
                        if (newState == state_)
                            return;
                        S();L("Changing state from " << state_ << " to " << newState);
                        state_ = newState;
                    }

                    int fid_;
                    State state_;
                    typedef std::set<int> Clients;
                    Clients clients_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
