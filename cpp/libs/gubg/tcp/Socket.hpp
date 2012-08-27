#ifndef gubg_tcp_Socket_hpp
#define gubg_tcp_Socket_hpp

#include "gubg/tcp/Codes.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>

namespace gubg
{
    namespace tcp
    {
        enum class Role {Client, Server};

        template <typename Receiver, Role RoleT>
            class Socket_crtp
            {
                private:
                    static const int InvalidFID = -1;
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
                                switch (RoleT)
                                {
                                    case Role::Server: state_ = NeedsBinding; break;
                                    case Role::Client: state_ = NeedsConnect; break;
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
                                state_ = NeedsListening;
                                break;
                            case NeedsListening:
                                {
                                    const int MaxNrWaitingConnections = 20;
                                    MSS(::listen(fid_, MaxNrWaitingConnections) != -1, CouldNotListen);
                                }
                                state_ = NeedsAccept;
                                break;
                            case NeedsAccept:
                                state_ = IsConnected;
                                break;
                            case IsConnected:
                                break;
                        }
                        MSS_END();
                    }

                private:
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                    ReturnCode getSocketDescriptor_()
                    {
                    }

                    int fid_;
                    enum State {NeedSocketDescriptor, NeedsBinding, NeedsListening, NeedsAccept, NeedsConnect, IsListening, HasAccepted, IsConnected};
                    State state_;
            };
    }
}

#endif
