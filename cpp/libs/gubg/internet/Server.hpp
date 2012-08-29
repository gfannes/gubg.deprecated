#ifndef gubg_internet_Server_hpp
#define gubg_internet_Server_hpp

#include "gubg/internet/Socket.hpp"
#include "gubg/internet/Endpoint.hpp"
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
                        port_(port){}

                    ReturnCode start()
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(socket_.bind(port));
                        MSS(socket_.listen());
                        {
                            continue_ = true;
                            std::thread thread(std::ref(*this));
                            thread_ = std::move(thread);
                        }
                        MSS_END();
                    }
                    ReturnCode stop()
                    {
                        continue_ = false;
                    }

                    void operator()()
                    {
                        MSS_BEGIN(void);
                        while (true)
                        {
                            MSS(continue_);
                            Socket peer;
                            MSS(socket_.accept(peer));
                            new Endpoint<Handler>(peer);
                        }
                        MSS_END();
                    }

                private:
                    Socket socket_;
                    bool continue_;
                    std::thread thread_;
            };
    }
}

#endif
