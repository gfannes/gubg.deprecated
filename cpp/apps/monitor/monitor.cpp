#include "monitor/Codes.hpp"
#include "monitor/Finalize.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Descriptor.hpp"
#include "gubg/Testing.hpp"
#include <iostream>
using namespace monitor;
using gubg::OptionParser;
using gubg::file::Descriptor;
using gubg::file::EventType;
using gubg::file::AccessMode;
using gubg::file::File;
using namespace std;

#define GUBG_MODULE_ "monitor::Select"
#include "gubg/log/begin.hpp"
namespace 
{
    class Select: public gubg::file::Select
    {
        public:
            int nr;
            Select():nr(0){}
            virtual bool select_ready(Descriptor d, EventType et)
            {
                MSS_BEGIN(bool, d << " is ready " << STREAM((int)et));
                L("select_ready " << STREAM(d, (int)et));
                switch (et)
                {
                    case EventType::Open:
                        MSS(d.accept(tty_));
                        LLL("Accepted a tty, adding it to the read set: " << tty_);
                        tty_.setBaudRate(9600);
                        add(tty_, AccessMode::ReadWrite);
                        break;
                    case EventType::Close:
                        LLL("Closed tty " << tty_);
                        erase(tty_, AccessMode::ReadWrite);
                        break;
                    case EventType::Read:
                        {
                            string buf(16, '\0');
                            MSS(d.read(buf));
                            //cout << buf.size() << " " << buf << " " << gubg::testing::toHex(buf) << endl;
                            if ((unsigned char)buf[0] == 0x83)
                                cout << std::endl;
                            cout << gubg::testing::toHex(buf);// << std::endl;
                            L(buf);
                        }
                        break;
                    case EventType::Write:
                        if (nr == 0)
                        {
                            nr = 1;
                            size_t s;
                            string str("Hello world");
                            LLL("Sending " << str);
                 //           MSS(d.write(s, str));
                            if (false && s == str.size())
                                erase(tty_, AccessMode::Write);
                        }
                        break;
                }
                MSS_END();
            }
        private:
            Descriptor tty_;
    };
}
#include "gubg/log/end.hpp"

#define GUBG_MODULE "monitor"
#include "gubg/log/begin.hpp"
namespace 
{

    ReturnCode main_(int argc, char **argv)
    {
        MSS_BEGIN(ReturnCode);

        {
            bool loadMindMap = false;
            OptionParser optionParser("Network and file monitor");
            optionParser.addSwitch("-h", "--help", "Displays this help", [&optionParser](){MONITOR_FINALIZE_OK(optionParser.help());});

            OptionParser::Args args;
            MSS(OptionParser::createArgs(args, argc, argv));
            MSS(optionParser.parse(args));
        }

        auto d = Descriptor::listen(File("/dev/ttyACM0"), AccessMode::ReadWrite);
        MSS(d.valid());
        Select s;
        s.add(d, AccessMode::Read);

        while (true)
            s(std::chrono::milliseconds(500));

        MSS_END();
    }
}

int main(int argc, char **argv)
{
    if (!gubg::mss::isOK(main_(argc, argv)))
        MONITOR_FINALIZE_ERROR("Problems were encountered...");
    MONITOR_FINALIZE_OK("Everything went OK");
    return 0;
}
