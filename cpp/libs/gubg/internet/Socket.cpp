#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/internet/Socket.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <cassert>
using namespace std;

namespace 
{
    const int InvalidFID = -1;
    const int MaxNrWaitingConnections = 20;
    enum State {Fresh, Bound, Listening, Connected, Closed};
}

namespace gubg
{
    namespace internet
    {
        struct Socket::Pimpl
        {
            int fid;
            State state;

            Pimpl(int f):
                fid(f),
                state(Fresh)
            {
                assert(fid != InvalidFID);
            }
            ~Pimpl()
            {
                assert(fid != InvalidFID);
                //We shutdown() the socket first, making sure blocking operations are unblocked
                shutdown();
                ::close(fid);
            }
            ReturnCode bind(int port)
            {
                MSS_BEGIN(ReturnCode);

                MSS(state == Fresh, InvalidState);

                MSS(port > 0, PortTooLow);
                MSS(port < 65536, PortTooHigh);
                std::ostringstream oss;
                oss << port;
                struct addrinfo hints, *res;
                ::memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_flags = AI_PASSIVE;
                MSS(::getaddrinfo(0, oss.str().c_str(), &hints, &res) == 0, CouldNotGetAddrInfo);
                MSS(::bind(fid, res->ai_addr, res->ai_addrlen) != -1, CouldNotBind);

                changeState(Bound);

                MSS_END();
            }
            ReturnCode listen()
            {
                MSS_BEGIN(ReturnCode);
                MSS(state == Bound, InvalidState);
                MSS(::listen(fid, MaxNrWaitingConnections) != -1, CouldNotListen);
                changeState(Listening);
                MSS_END();
            }
            ReturnCode accept(int &f)
            {
                MSS_BEGIN(ReturnCode);
                MSS(state == Listening, InvalidState);
                struct sockaddr_storage peer;
                socklen_t peerSize = sizeof(peer);
                int peerFid = ::accept(fid, (struct sockaddr *)&peer, &peerSize);
                MSS(peerFid != -1, CouldNotAccept);
                f = peerFid;
                MSS_END();
            }
            ReturnCode connect(const string &ip, int port)
            {
                MSS_BEGIN(ReturnCode);

                MSS(state == Fresh, InvalidState);

                MSS(port > 0, PortTooLow);
                MSS(port < 65536, PortTooHigh);

                struct addrinfo hints, *res;
                int sockfd;
                ::memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_UNSPEC; //Use IPv4 or IPv6, whichever
                hints.ai_socktype = SOCK_STREAM;
                ostringstream oss;
                oss << port;
                MSS(::getaddrinfo(ip.c_str(), oss.str().c_str(), &hints, &res) == 0, CouldNotGetAddrInfo);
                MSS(::connect(fid, res->ai_addr, res->ai_addrlen) != -1, CouldNotConnect);

                changeState(Connected);

                MSS_END();
            }
            template <typename IOBuffer>
            ReturnCode send(IOBuffer &buffer)
            {
                MSS_BEGIN(ReturnCode);
                MSS(state == Connected, InvalidState);
                auto nrSent = ::send(fid, buffer.data(), buffer.size(), 0);
                MSS(nrSent != -1, CouldNotSend);
                buffer.scrollBegin(nrSent);
                MSS_END();
            }
            template <typename IOBuffer>
            ReturnCode receive(IOBuffer &buffer)
            {
                MSS_BEGIN(ReturnCode);
                MSS(state == Connected, InvalidState);
                MSS(!buffer.full());
                auto nrReceived = ::recv(fid, buffer.freeData(), buffer.freeSize(), 0);
                MSS(nrReceived != -1, CouldNotReceive);
                if (nrReceived == 0)
                {
                    //Peer closed connection
                    changeState(Closed);
                    MSS_Q(ReturnCode::ConnectionWasClosed);
                }
                else
                    MSS(buffer.scrollEnd(nrReceived));
                MSS_END();
            }
            ReturnCode shutdown()
            {
                MSS_BEGIN(ReturnCode);
                ::shutdown(fid, SHUT_RDWR);
                MSS_END();
            }
            void changeState(State newState)
            {
                if (newState == state)
                    return;
                LOG_S(changeState_, "Changing state from " << state << " to " << newState);
                state = newState;
            }
        };

        //Public API
        ReturnCode Socket::bind(int port)
        {
            MSS_BEGIN(ReturnCode);
            MSS(createSocket_());
            MSS(pimpl_->bind(port));
            MSS_END();
        }
        ReturnCode Socket::listen()
        {
            MSS_BEGIN(ReturnCode);
            MSS(pimpl_);
            MSS(pimpl_->listen());
            MSS_END();
        }
        ReturnCode Socket::accept(Socket &socket)
        {
            MSS_BEGIN(ReturnCode);
            MSS(pimpl_);
            int fid;
            MSS(pimpl_->accept(fid));
            socket = Socket(fid);
            MSS_END();
        }
        ReturnCode Socket::connect(const string &ip, int port)
        {
            MSS_BEGIN(ReturnCode);
            //TODO::In stead of splitting the socket creation and connect, this should be combined
            //to allow us to use the PF_INET/PF_INET6 settings resolved by getaddrinfo and to be used in
            //the socket creation
            MSS(createSocket_());
            MSS(pimpl_->connect(ip, port));
            MSS_END();
        }
        ReturnCode Socket::send(IOBuffer &buffer)
        {
            MSS_BEGIN(ReturnCode);
            MSS(pimpl_);
            MSS(pimpl_->send(buffer));
            MSS_END();
        }
        ReturnCode Socket::receive(IOBuffer &buffer)
        {
            MSS_BEGIN(ReturnCode);
            MSS(pimpl_);
            MSS(pimpl_->receive(buffer));
            MSS_END();
        }

        ReturnCode Socket::shutdown()
        {
            MSS_BEGIN(ReturnCode);
            MSS(pimpl_);
            MSS(pimpl_->shutdown());
            MSS_END();
        }

        //Private API
        Socket::Socket(int fid):
            pimpl_(new Pimpl(fid))
        {
            pimpl_->changeState(Connected);
        }

        ReturnCode Socket::createSocket_()
        {
            MSS_BEGIN(ReturnCode);
            MSS(!pimpl_, SocketAlreadyPresent);
            int fid = ::socket(PF_INET, SOCK_STREAM, 0);
            MSS(fid != InvalidFID, CouldNotGetSocketDescriptor);
            pimpl_.reset(new Pimpl(fid));
            MSS_END();
        }
    }
}
