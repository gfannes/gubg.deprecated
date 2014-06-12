#include "gubg/file/Descriptor.hpp"
#include "gubg/String.hpp"
#include "gubg/Timer.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/d9/D9.hpp"
#include <queue>

enum class ReturnCode {MSS_DEFAULT_CODES, };

#define GUBG_MODULE_ "rvc"
#include "gubg/log/begin.hpp"
namespace rvc { 

    class Model: public gubg::file::Select
    {
        public:
            Model(gubg::file::File flamingo):
                sendMessage_(*this, std::chrono::milliseconds(500))
            {
                flamingo_listen_ = Descriptor::listen(flamingo, AccessMode::ReadWrite);
                add(flamingo_listen_, AccessMode::Read);
            }

            void process()
            {
                //S();
                Select::process(std::chrono::milliseconds(100));
                sendMessage_.process();
            }

        private:
            typedef gubg::file::Descriptor Descriptor;
            typedef gubg::file::AccessMode AccessMode;
            typedef gubg::file::EventType EventType;

            virtual void select_ready(Descriptor descr, EventType eventType)
            {
                MSS_BEGIN(void, descr << " " << to_hr(eventType));
                switch (eventType)
                {
                    case EventType::Open:
                        L("Someone is trying to connect");
                        if (false) {}
                        else if (descr == flamingo_listen_)
                        {
                            MSS(!flamingo_descr_.valid());
                            MSS(descr.accept(flamingo_descr_));
                            MSS(flamingo_descr_.setBaudRate(9600));
                            MSS(add(flamingo_descr_, AccessMode::Read));
                            L("Flamingo is connected");
                            std::this_thread::sleep_for(std::chrono::seconds(2));
                        }
                        break;
                    case EventType::Read:
                        L("Descriptor is ready for reading");
                        {
                            std::string buffer(1024, '\0');
                            {
                                const auto rc = descr.read(buffer);
                                if (rc == gubg::file::ReturnCode::PeerClosedConnection)
                                {
                                    L("Peer closed the connection, I will throw him out of my select");
                                    MSS(erase(descr, AccessMode::Read));
                                    MSS_RETURN_OK();
                                }
                                MSS(rc);
                                MSS(!buffer.empty());
                            }

                            if (false) {}
                            else if (descr == flamingo_descr_)
                            {
                                L("I received " << buffer.size() << " bytes from flamingo: " << buffer);
                            }
                        }
                        break;
                    case EventType::Write:
                        L("Descriptor is ready for writing");
                        MSS(descr == flamingo_descr_);
                        if (messages_4_flamingo_.empty())
                        {
                            L("No more messages to send, removing flamingo from Write set");
                            MSS(erase(flamingo_descr_, AccessMode::Write));
                            MSS_RETURN_OK();
                        }
                        {
                            size_t nrWritten = 0;
                            auto &msg = messages_4_flamingo_.front();
                            L("Sending msg: " << gubg::toHex(msg));
                            MSS(flamingo_descr_.write(nrWritten, msg));
                            if (nrWritten == msg.size())
                            {
                                messages_4_flamingo_.pop();
                                if (messages_4_flamingo_.empty())
                                {
                                    L("No more messages to send, removing flamingo from Write set");
                                    MSS(erase(flamingo_descr_, AccessMode::Write));
                                    MSS_RETURN_OK();
                                }
                            }
                            else
                                msg = msg.substr(nrWritten);
                        }
                        break;
                }
                MSS_END();
            }

            Descriptor flamingo_listen_;
            Descriptor flamingo_descr_;

            typedef std::queue<std::string> Messages;
            Messages messages_4_flamingo_;

            typedef gubg::msgpack::Serializer<std::string, 5> Serializer;
            Serializer serializer;
            void timer_expired()
            {
                S();L("timer_expired");
                //Reschedule the timer
                sendMessage_.reset();

                if (!flamingo_descr_.valid())
                {
                    L("Flamingo not connected yet");
                    return;
                }

                const int ut = gubg::chrono::uptime()/5;
                if (ut%2 == 0)
                {
                    //Send keep alive
                    serializer.clear();
                    serializer.serialize(nullptr);
                    std::string msg;
                    gubg::d9::encode(msg, serializer.buffer());
                    messages_4_flamingo_.push(msg);
                    if (messages_4_flamingo_.size() == 1)
                    {
                        L("Adding flamingo to Write set");
                        add(flamingo_descr_, AccessMode::Write);
                    }
                }
            }
            friend class gubg::Timer_ftop<Model>;
            typedef gubg::Timer_ftop<Model> SendMessage;
            SendMessage sendMessage_;
    };

} 

ReturnCode main_()
{
    MSS_BEGIN(ReturnCode);

    rvc::Model model(gubg::file::File("/dev/ttyACM0"));
    while (true)
    {
        model.process();
    }

    MSS_END();
}
int main()
{
    return (MSS_IS_OK(main_()) ? 0 : -1);
}

#if 0
#include "gubg/log/end.hpp"
#include "gubg/mss.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/d9/D9.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/tty/Endpoint.hpp"
#include "gubg/Timer.hpp"
#include "garf/Types.hpp"
#include "SDL/SDL.h"
#include <iostream>
#include <thread>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Rover"
#include "gubg/log/begin.hpp"
enum class ReturnCode
{
    MSS_DEFAULT_CODES,
    NoJoystickFound, CouldNotOpenJoystick
};

namespace 
{
    int nrJoysticks;
    SDL_Joystick *joystick = 0;
    class Arduino: public gubg::tty::Endpoint_crtp<Arduino>
    {
        public:
            Arduino():Endpoint_crtp("/dev/ttyACM0"){}
            void endpoint_received(unsigned char ch)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)ch << '|';
                std::cout.flush();
            }
    };
    class KeepAlive: public gubg::Timer_crtp<KeepAlive>
    {
        public:
            garf::pod::Motor motor;

            KeepAlive(Arduino &arduino):arduino_(arduino){}
            void timer_expired()
            {
                S();
                if (false)
                {
                    L("Stay awake man");
                    serializer_.clear();
                    serializer_.serialize(nullptr);
                }
                else
                {
                    serializer_.frame(motor);
                }

                std::string msg;
                gubg::d9::encode(msg, serializer_.buffer());
                L(testing::toHex(msg));
                arduino_.send(msg);

                reset();
            }
        private:
            Arduino &arduino_;
            typedef gubg::msgpack::Serializer<std::string, 5> Serializer;
            Serializer serializer_;
    };
}

ReturnCode setup()
{
    MSS_BEGIN(ReturnCode);
    MSS(SDL_Init(SDL_INIT_EVERYTHING) != -1);
    nrJoysticks = SDL_NumJoysticks();
    MSS(nrJoysticks > 0, NoJoystickFound);
    joystick = SDL_JoystickOpen(0);
    MSS(joystick, CouldNotOpenJoystick);
    MSS_END();
}
void teardown()
{
    if (joystick)
        SDL_JoystickClose(joystick);
    SDL_Quit();
}
ReturnCode poll()
{
    MSS_BEGIN(ReturnCode);
    bool quit = false;

    Arduino arduino;
    KeepAlive keepAlive(arduino);
    keepAlive.setTimeout(std::chrono::milliseconds(500));

    vector<int> directions(2);

    while (!quit)
    {
        arduino.process();
        keepAlive.process();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
                break;
            }

            if (event.type == SDL_JOYAXISMOTION)
            {
                switch (event.jaxis.axis)
                {
                    case 0:
                        //L("X " << event.jaxis.value);
                        directions[0] = event.jaxis.value;
                        break;
                    case 1:
                        //L("Y " << event.jaxis.value);
                        directions[1] = event.jaxis.value;
                        break;
                }
                keepAlive.motor.left = (+directions[0]-directions[1])/800;
                keepAlive.motor.right = (-directions[0]-directions[1])/800;
            }
        }
    }
    MSS_END();
}
ReturnCode main_()
{
    MSS_BEGIN(ReturnCode);
    MSS(setup());
    MSS(poll());
    teardown();
    MSS_END();
}

int main()
{
    return (mss::isOK(main_()) ? 0 : -1);
}
#include "gubg/log/end.hpp"
#endif
