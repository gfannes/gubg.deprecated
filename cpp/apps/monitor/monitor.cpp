#include "monitor/Codes.hpp"
#include "monitor/Finalize.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Descriptor.hpp"
#include <iostream>
using namespace monitor;
using gubg::OptionParser;
using gubg::file::Descriptor;
using gubg::file::EventType;
using gubg::file::AccessMode;
using gubg::file::File;
using namespace std;

#define GUBG_MODULE "monitor::Select"
#include "gubg/log/begin.hpp"
namespace 
{
    class Select: public gubg::file::Select
    {
        public:
            virtual bool select_ready(Descriptor d, EventType et)
            {
                MSS_BEGIN(bool, d << " is ready to read");
                L("select_ready " << STREAM(d, (int)et));
                switch (et)
                {
                    case EventType::Open:
                        MSS(d.accept(tty_));
                        LLL("Accepted a tty, adding it to the read set: " << tty_);
                        add(tty_, AccessMode::Read);
                        break;
                    case EventType::Close:
                        break;
                    case EventType::Read:
                        {
                            string buf(16, '\0');
                            MSS(d.read(buf));
                            cout << buf;
                            L(buf);
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
            s(std::chrono::seconds(1));

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
