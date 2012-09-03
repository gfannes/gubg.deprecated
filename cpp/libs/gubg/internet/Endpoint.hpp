#ifndef gubg_internet_Endpoint_hpp
#define gubg_internet_Endpoint_hpp

#include "gubg/internet/Socket.hpp"
#include <string>
#include <thread>
#include <mutex>
#include <memory>

namespace gubg
{
    namespace internet
    {
        template <typename Receiver>
            class Endpoint_crtp
            {
                public:
                    typedef std::shared_ptr<Receiver> Ptr;

                    Endpoint_crtp(Socket socket):
                        socket_(socket),
                        doReceive_(true),
                        senderIsRunning_(false),
                        thread_(std::ref(*this))
                {
                }
                    ~Endpoint_crtp()
                    {
                        doReceive_ = false;
                        socket_ = Socket();
                        thread_.join();
                    }

                    ReturnCode send(const std::string &msg)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(!msg.empty());
                        mutex_.lock();
                        out_.append(msg);
                        if (!senderIsRunning_)
                        {
                            senderIsRunning_ = true;
                            //sendBuffer_ will unlock the mutex_
                            //We detach the thread to allow the thread to run on its own
                            std::thread(sendBuffer_, socket_, std::ref(senderIsRunning_), std::ref(out_), std::ref(mutex_)).detach();
                        }
                        else
                            //Sender is running, it will keep on sending until out_ is empty
                            mutex_.unlock();
                        MSS_END();
                    }
                    //sendBuffer_ assumes outMutex is locked when it is received
                    //It will copy out into an IOBuffer, unlock outMutex and send the IOBuffer
                    //If another send is issued while this thread is running, sendBuffer_ will send also what was scheduled
                    static ReturnCode sendBuffer_(Socket socket, volatile bool &senderIsRunning, std::string &out, std::mutex &mut)
                    {
                        MSS_BEGIN(ReturnCode);
                        assert(senderIsRunning);
                        assert(!out.empty());
                        while (senderIsRunning)
                        {
                            //Move data from out to buffer
                            Socket::IOBuffer buffer(out);
                            out.clear();

                            //We send buffer while out is unlocked, other sends can now add new data
                            mut.unlock();
                            while (!buffer.empty())
                            {
                                switch (auto rc = socket.send(buffer))
                                {
                                    case ReturnCode::OK: break;
                                    default:
                                                         //Something went wrong with sending, we indicate that the sender is gone and exit
                                                         {
                                                             std::lock_guard<std::mutex> lock(mut);
                                                             senderIsRunning = false;
                                                         }
                                                         MSS(rc);
                                                         break;
                                }
                            }

                            //We lock again to check if new data for sending has arrived, and set senderIsRunning to false if it has not
                            mut.lock();
                            senderIsRunning = !out.empty();
                        }
                        mut.unlock();
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
                            MSS(receiver_().endpoint_receive(buffer.str()));
                        }
                        MSS_END();
                    }

                private:
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                    Socket socket_;

                    //The out_ buffer (data to be sent) and the senderIsRunning_ are both protected by the mutex
                    std::string out_;
                    volatile bool senderIsRunning_;
                    std::mutex mutex_;

                    volatile bool doReceive_;
                    std::thread thread_;
            };
    }
}

#endif
