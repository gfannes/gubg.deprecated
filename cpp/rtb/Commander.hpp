#ifndef HEADER_rtb_Commander_hpp_ALREADY_INCLUDED
#define HEADER_rtb_Commander_hpp_ALREADY_INCLUDED

#include "Messagetypes.h"
#include "gubg/SmartRange.hpp"
#include <iostream>
#include <fstream>
#include <string>

namespace rtb
{
    template <typename Receiver>
    class Commander_crtp
    {
        public:
            Commander_crtp(const std::string &name): name_(name)
        {
            log_stream_.open(name_ + ".log", std::ios_base::out | std::ios_base::trunc);
            std::cout << "RobotOption " << USE_NON_BLOCKING << " " << 0 << std::endl;
        }

            bool process()
            {
                processMessage_(readMessage_());
                return continue_;
            }

        private:
            typedef gubg::SmartRange<std::string> Range;
            Receiver &receiver_() {return static_cast<Receiver&>(*this);}

            const std::string homeColor = "ff0000";
            const std::string awayColor = "0000ff";

            std::string readMessage_()
            {
                std::string str;
                std::getline(std::cin, str);
                log_stream_ << str << std::endl;
                return str;
            }
            void processMessage_(std::string &&msg)
            {
                Range range(std::move(msg));
                Range cmd;
                if (!range.pop(cmd, ' '))
                {
                    log_stream_ << "Could not pop command" << std::endl;
                    continue_ = false;
                    return;
                }
                log_stream_ << std::string(cmd.data(), cmd.size()) << std::endl;
            }

            const std::string name_;
            bool continue_ = true;
            std::ofstream log_stream_;
#if 0
    string cmd;
    int seq;
    cin >> cmd >> seq;
    if (seq == 1)
    {
        cout << "Name " << name << endl;
        cout << "Colour " << homeColor << " " << awayColor << endl;
    }
#endif
    };
}

#endif
