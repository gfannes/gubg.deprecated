#ifndef gubg_internet_Endpoint_hpp
#define gubg_internet_Endpoint_hpp

#include "gubg/internet/Socket.hpp"
#include <string>

namespace gubg
{
    namespace internet
    {
        template <typename Handler>
            class Endpoint
            {
                public:
                    Endpoint(Socket socket):
                        socket_(socket),
                        continue_(true),
                        thread_(std::ref(*this))
                {
                }

                    ReturnCode send(const std::string &msg)
                    {
                        MSS_BEGIN(ReturnCode);
                        std::unique_lock<std::mutex> lock(outStagingMutex_);
                        outStaging_.append(msg);
                        newDataArrived_.notify_one();
                        MSS_END();
                    }

                    void operator()()
                    {
                        MSS_BEGIN(void);
                        while (true)
                        {
                            MSS(continue_);
                            {
                                std::unique_lock<std::mutex> lock(outStagingMutex_);
                                while (outStaging_.empty())
                                    newDataArrived_.wait(lock);
                                out_.assign(outStaging_);
                                outStaging_.clear();
                            }
                            while (!out_.empty())
                                MSS(socket_.send(out_));
                        }
                        MSS_END();
                    }

                private:
                    Socket socket_;
                    Socket::IOBuffer out_;
                    std::string outStaging_;
                    std::mutex outStagingMutex_;
                    std::condition_variable newDataArrived_;
                    std::string in_;
                    bool continue_;
                    std::thread thread_;
            };
    }
}

#endif
