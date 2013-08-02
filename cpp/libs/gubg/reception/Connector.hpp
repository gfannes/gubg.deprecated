#ifndef HEADER_gubg_reception_Connector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_reception_Connector_hpp_ALREADY_INCLUDED

#include "gubg/reception/Codes.hpp"
#include "ev++.h"
#include <thread>

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
                    MSS(acceptorPerPort_.count(port) == 0);
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
                            return Ptr(new Acceptor(p));
                        }
                    private:
                        Acceptor(int p): port(p) { }
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
