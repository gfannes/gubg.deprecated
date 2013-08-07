#ifndef HEADER_gubg_reception_Connector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_reception_Connector_hpp_ALREADY_INCLUDED

#include "gubg/reception/Codes.hpp"
#include "ev++.h"
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define GUBG_MODULE_ "Connector"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace reception
    {
        class Connector
        {
            public:
                Connector():
                    quit_(false),
                    thread_(std::ref(*this)){}
                ~Connector()
                {
                    quit_ = true;
                    thread_.join();
                }

                ReturnCode watch(int port)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS(acceptorPerPort_.count(port) == 0, PortAlreadyWatched);
                    acceptorPerPort_[port] = Acceptor::create(port);
                    MSS_END();
                }

                void operator()()
                {
                    S();
                    L("Starting the default event loop");
                    ev::default_loop loop;
                    while (!quit_)
                        loop.run(0);
                }

            private:
                class Acceptor
                {
                    public:
                        const int port;
                        typedef std::unique_ptr<Acceptor> Ptr;
                        static Ptr create(int p)
                        {
                            Ptr res(new Acceptor(p));
                            if (!mss::isOK(res->valid()))
                                res.reset();
                            return res;
                        }

                        ReturnCode valid() const {return valid_;}

                        void io_accept(ev::io &watcher, int revents)
                        {
                            S();
                            L("Received an event");
                            if (EV_ERROR & revents)
                            {
                                L("Received an invalid event");
                                return;
                            }

                            struct sockaddr_in client_addr;
                            socklen_t client_len = sizeof(client_addr);

                            int client_sd = ::accept(watcher.fd, (struct sockaddr *)&client_addr, &client_len);

                            if (client_sd < 0)
                            {
                                L("Failed to accept the incomming connection");
                                return;
                            }

                            new Socket(client_sd);
                        }
                        static void signal_cb(ev::sig &signal, int revents)
                        {
                            signal.loop.break_loop();
                        }


                    private:
                        Acceptor(int p):
                            port(p), valid_(ReturnCode::OK)
                    {
                        S();
                        L("Installing acceptor for " << STREAM(port));
                        struct sockaddr_in addr;

                        descr_ = ::socket(PF_INET, SOCK_STREAM, 0);

                        addr.sin_family = AF_INET;
                        addr.sin_port     = htons(port);
                        addr.sin_addr.s_addr = INADDR_ANY;

                        if (::bind(descr_, (struct sockaddr *)&addr, sizeof(addr)) != 0)
                        {
                            L("Failed to bind");
                            valid_ = ReturnCode::CouldNotBind;
                            return;
                        }

                        ::fcntl(descr_, F_SETFL, ::fcntl(descr_, F_GETFL, 0) | O_NONBLOCK); 

                        ::listen(descr_, 5);

                        io_.set<Acceptor, &Acceptor::io_accept>(this);
                        io_.start(descr_, ev::READ);

                        sio_.set<&Acceptor::signal_cb>();
                    }
                        class Socket
                        {
                            public:
                                Socket(int descr): descr_(descr)
                            {
                                S();L("Got connection");
                                ::fcntl(descr_, F_SETFL, ::fcntl(descr_, F_GETFL, 0) | O_NONBLOCK); 
                                io_.set<Socket, &Socket::callback>(this);
                                io_.start(descr_, ev::READ);
                            }
                                void callback(ev::io &watcher, int revents)
                                {
                                    S();L("Activity on a Socket");
                                    if (EV_ERROR & revents)
                                    {
                                        L("got invalid event");
                                        return;
                                    }

                                    if (revents & EV_READ) 
                                        read_(watcher);

#if 0
                                    if (revents & EV_WRITE) 
                                        write_(watcher);

                                    if (write_queue.empty()) {
                                        io.set(ev::READ);
                                    } else {
                                        io.set(ev::READ|ev::WRITE);
                                    }
#endif
                                }

                            private:
                                void read_(ev::io &watcher)
                                {
                                    S();L("Reading");
                                    char buffer[1024];

                                    const ssize_t nr = ::recv(watcher.fd, buffer, sizeof(buffer)-1, 0);

                                    if (nr < 0) {
                                        L("read error");
                                        return;
                                    }
                                    L(STREAM(nr));

                                    if (nr == 0)
                                    {
                                        L("Peer closed the connection");
                                        // Gack - we're deleting ourself inside of ourself!
                                        //delete this;
                                        return;
                                    }
                                    buffer[nr] = 0x00;

                                    L(STREAM(buffer));

                                    std::string msg("<html><body>Hello world</body></html>");
                                    ::write(watcher.fd, msg.data(), msg.size());
                                }
                                ev::io io_;
                                int descr_;
                        };

                        ReturnCode valid_;
                        int descr_;
                        ev::io io_;
                        ev::sig sio_;
                };
                typedef std::map<int, Acceptor::Ptr> AcceptorPerPort;
                AcceptorPerPort acceptorPerPort_;

                volatile bool quit_;
                std::thread thread_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
