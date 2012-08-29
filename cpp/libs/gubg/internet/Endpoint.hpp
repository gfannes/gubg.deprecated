#ifndef gubg_internet_Endpoint_hpp
#define gubg_internet_Endpoint_hpp

#include "gubg/internet/Socket.hpp"
#include <string>
#include <thread>
#include <mutex>

namespace gubg
{
    namespace internet
    {
        template <typename Handler>
            class Endpoint: public Handler
            {
                public:
                    Endpoint(Socket socket):
                        socket_(socket),
                        doReceive_(true),
                        thread_(std::ref(*this))
                {
                }
                    ~Endpoint()
                    {
                        doReceive_ = false;
                        socket_ = Socket();
                        thread_.join();
                    }

                    ReturnCode send(const std::string &msg)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_L(NotImplemented);
#if 0
                        std::unique_lock<std::mutex> lock(outMutex_);
                        out_.append(msg);
                        newDataArrived_.notify_one();
#endif
                        MSS_END();
                    }

                    void operator()()
                    {
                        MSS_BEGIN(void, receive_thread);
                        while (true)
                        {
                            MSS(doReceive_);
                            Socket::IOBuffer buffer(1024);
                            MSS(socket_.receive(buffer));
                            MSS(Handler::receive(buffer.str()));
#if 0
                            {
                                std::unique_lock<std::mutex> lock(outMutex_);
                                while (out_.empty())
                                    newDataArrived_.wait(lock);
                                out_.assign(out_);
                                out_.clear();
                            }
                            while (!out_.empty())
                                MSS(socket_.send(out_));
#endif
                        }
                        MSS_END();
                    }

                private:
                    Socket socket_;
                    std::string out_;
                    //std::mutex outMutex_;
                    //std::condition_variable newDataArrived_;
                    volatile bool doReceive_;
                    std::thread thread_;
            };
    }
}

#endif
