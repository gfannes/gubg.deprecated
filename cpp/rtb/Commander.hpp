#ifndef HEADER_rtb_Commander_hpp_ALREADY_INCLUDED
#define HEADER_rtb_Commander_hpp_ALREADY_INCLUDED

#include "rtb/Messages.hpp"
#include "Messagetypes.h"
#include "gubg/SmartRange.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>

namespace rtb
{
    typedef gubg::SmartRange<std::string> Range;

    template <typename Receiver>
    class Commander_crtp
    {
        private:
        public:
            std::ofstream log;

            Commander_crtp(const std::string &name): name_(name), prev_(Clock::now())
        {
            log.open(name_ + ".log", std::ios_base::out | std::ios_base::trunc);
            std::cout << "RobotOption " << USE_NON_BLOCKING << " " << 0 << std::endl;
        }

            bool process()
            {
                processMessage_(readMessage_());
                return continue_;
            }

        private:
            Receiver &receiver_() {return static_cast<Receiver&>(*this);}

            const std::string homeColor = "ff0000";
            const std::string awayColor = "0000ff";

            std::string readMessage_()
            {
                std::string str;
                std::getline(std::cin, str);
                log << diff_() << " " << str << std::endl;
                return str;
            }
            void processMessage_(std::string &&msg)
            {
                Range range(std::move(msg));
                Range cmd;
                if (!range.pop(cmd, ' '))
                {
                    log << "Could not pop command" << std::endl;
                    continue_ = false;
                    return;
                }
#define L_CHECK_CMD(command) \
                if (cmd.size() == sizeof(#command)-1 && std::memcmp(cmd.data(), #command, cmd.size()) == 0) \
                { \
                    command c; \
                    createCommand_(c, range); \
                    receiver_().commander_event(c); \
                }
                L_CHECK_CMD(Initialize);
                L_CHECK_CMD(GameOption);
                L_CHECK_CMD(Warning);
                L_CHECK_CMD(GameStarts);
                L_CHECK_CMD(Radar);
                L_CHECK_CMD(Coordinates);
#undef L_CHECK_CMD
            }
            void createCommand_(Initialize &cmd, Range &args)
            {
                args.pop(cmd.seq);
            }
            void createCommand_(GameOption &cmd, Range &args)
            {
                args.pop(cmd.id);
                args.pop(cmd.value);
            }
            void createCommand_(Warning &cmd, Range &args)
            {
                args.pop(cmd.id);
            }
            void createCommand_(GameStarts &cmd, Range &args)
            {
            }
            void createCommand_(Radar &cmd, Range &args)
            {
                args.pop(cmd.distance);
                int t;
                args.pop(t);
                cmd.type = (ObjectType)t;
                args.pop(cmd.angle);
            }
            void createCommand_(Coordinates &cmd, Range &args)
            {
                args.pop(cmd.x);
                args.pop(cmd.y);
                args.pop(cmd.angle);
            }

            unsigned long diff_()
            {
                auto now = Clock::now();
                unsigned long res = (now - prev_).count();
                prev_ = now;
                return res;
            }

            const std::string name_;
            bool continue_ = true;
            typedef std::chrono::system_clock Clock;
            Clock::time_point prev_;
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
