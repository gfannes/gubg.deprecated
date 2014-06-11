#ifndef HEADER_gubg_tty_Endpoint_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tty_Endpoint_hpp_ALREADY_INCLUDED

#include "gubg/crtp.hpp"
#include <sstream>
#include <string>
#include <deque>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define GUBG_MODULE_ "tty::Endpoint"
#include "gubg/log/begin.hpp"
namespace gubg { namespace tty {

    template <typename Receiver>
        class Endpoint_crtp
        {
            private:
                static const int InvalidFD = -1;

            public:
                Endpoint_crtp(const std::string &devName):
                    devName_(devName),
                    fi_(InvalidFD),
                    ix_(0){}

                void send(const std::string &str)
                {
                    sendQueue_.push_back(str);
                }

                void process()
                {
                    if (!connect_())
                        return;
                    process_();
                }

            private:
                GUBG_RECEIVER();

                bool connect_()
                {
                    if (fi_ != InvalidFD)
                        return true;

                    fi_ = ::open(devName_.c_str(), O_RDWR | O_NONBLOCK);
                    if (fi_ == InvalidFD)
                        return false;

                    const auto baudRate = 9600;
                    std::ostringstream cmd;
                    cmd << "stty -F " << devName_ << " " << baudRate << " raw";
                    if (system(cmd.str().c_str()))
                        return false;

                    return fi_ != 0;
                }

                void process_()
                {
                    S();
                    while (!sendQueue_.empty())
                    {
                        L(STREAM(sendQueue_.size()));
                        if (ix_ == sendQueue_.front().size())
                        {
                            L("Message was sent");
                            sendQueue_.pop_front();
                            ix_ = 0;
                            break;
                        }
                        if (::write(fi_, &sendQueue_.front()[ix_++], 1) != 1)
                        {
                            L("Could not send byte");
                            break;
                        }
                    }
                    unsigned char ch;
                    switch (::read(fi_, &ch, 1))
                    {
                        case InvalidFD: L("Reading error: " << strerror(errno)); break;
                        case 0: L("Nothing read"); break;
                        case 1: receiver_().endpoint_received(ch); break;
                        default: L("Reading error 2"); break;
                    }
                }

                const std::string devName_;
                int fi_;
                typedef std::deque<std::string> SendQueue;
                SendQueue sendQueue_;
                size_t ix_;
        };

} }
#include "gubg/log/end.hpp"

#endif
