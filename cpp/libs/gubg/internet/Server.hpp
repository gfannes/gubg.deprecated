#ifndef gubg_internet_Server_hpp
#define gubg_internet_Server_hpp

#include "gubg/internet/Socket.hpp"
#include <thread>

namespace gubg
{
    namespace internet
    {
        template <typename Handler>
            class Server
            {
                public:
                    Server(int port):
                        port_(port),
                        doAccept_(false){}

                    ReturnCode start()
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(socket_.bind(port_));
                        MSS(socket_.listen());
                        {
                            doAccept_ = true;
                            std::thread thread(std::ref(*this));
                            thread_ = std::move(thread);
                        }
                        MSS_END();
                    }
                    ReturnCode stop()
                    {
                        MSS_BEGIN(ReturnCode, stop);
                        MSS(doAccept_);
                        doAccept_ = false;
                        //Reset socket_, the dtor of Socket::Pimpl will call ::shutdown(), which will unblock the blocking accept
                        socket_ = Socket();
                        thread_.join();
                        MSS_END();
                    }

                    void operator()()
                    {
                        MSS_BEGIN(void, accept_thread);
                        while (true)
                        {
                            MSS(doAccept_);
                            Socket peer;
                            MSS(socket_.accept(peer));
                            LOG_M("Accepted a new connection");
                            new Handler(peer);
                        }
                        MSS_END();
                    }

                private:
                    int port_;
                    volatile bool doAccept_;
                    Socket socket_;
                    std::thread thread_;
            };
    }
}

#endif
