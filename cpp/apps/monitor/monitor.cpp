#include "monitor/Codes.hpp"
#include "monitor/Finalize.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Descriptor.hpp"
#include "gubg/msgpack/Factory.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/d9/Decoder.hpp"
#include "gubg/Testing.hpp"
#include "garf/Types.hpp"
#include "garf/Types_stream.hpp"
#include <iostream>
using namespace monitor;
using gubg::OptionParser;
using gubg::file::Descriptor;
using gubg::file::EventType;
using gubg::file::AccessMode;
using gubg::file::File;
using namespace std;


#define GUBG_MODULE "Factory"
#include "gubg/log/begin.hpp"
namespace 
{
    class Factory: public gubg::msgpack::Factory_crtp<Factory, string, 5>
    {
        public:
            template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, RoleId rid)
            {
                S();L(STREAM(rid));
                switch (rid)
                {
                    case garf::pod::TypeIds::Time:    L("Time");    obj = wrap(time_);    break;
                    case garf::pod::TypeIds::TopInfo: L("TopInfo"); obj = wrap(topInfo_); break;
                }
            }
            void msgpack_createdObject(RoleId rid)
            {
                S();L(STREAM(rid));
                switch (rid)
                {
                    case garf::pod::TypeIds::Time:    std::cout << time_ << std::endl;    break;
                    case garf::pod::TypeIds::TopInfo: std::cout << topInfo_ << std::endl; break;
                }
            }
            void msgpack_set(gubg::msgpack::RoleId rid, long v)
            {
                S();L(STREAM(rid, v));
            }
            void msgpack_set(gubg::msgpack::RoleId rid, gubg::msgpack::Nil_tag)
            {
                S();L("nil");
            }
            void msgpack_set(gubg::msgpack::RoleId rid, const string &str)
            {
                S();L(STREAM(rid, str));
            }
        private:
            garf::pod::Time time_;
            garf::pod::TopInfo topInfo_;
    };
}
#include "gubg/log/end.hpp"


#define GUBG_MODULE "D9Decoder"
#include "gubg/log/begin.hpp"
namespace 
{
    class D9Decoder: public gubg::d9::Decoder_crtp<D9Decoder, std::string>
    {
        public:
            D9Decoder(Factory &f): factory_(f){}

            void d9_start()
            {
                S();L("");
                factory_.reset();
            }
            void d9_ubyte(unsigned char b)
            {
                S();L((int)b);
                factory_.process(b);
            }
        private:
            Factory &factory_;
    };
}
#include "gubg/log/end.hpp"


#define GUBG_MODULE "monitor::Select"
#include "gubg/log/begin.hpp"
namespace 
{
    enum Mode {Raw, Msgpack, D9_Msgpack};
    class Select: public gubg::file::Select
    {
        public:
            int nr;

            Select():
                nr(0),
                mode_(D9_Msgpack),
                d9Decoder_(factory_)
        {
            add(Descriptor::std_in(), AccessMode::Read);
        }

            void setMode(Mode m) {mode_ = m;}

            virtual void select_ready(Descriptor d, EventType et)
            {
                MSS_BEGIN(void, d << " is ready " << STREAM((int)et));
                L("select_ready " << STREAM(d, (int)et));
                switch (et)
                {
                    case EventType::Open:
                        MSS(d.accept(tty_));
                        LLL("Accepted a tty, adding it to the read set: " << tty_);
                        tty_.setBaudRate(9600);
                        add(tty_, AccessMode::ReadWrite);
                        break;
                    case EventType::CloseRead:
                        LLL("Closed tty " << tty_);
                        erase(tty_, AccessMode::ReadWrite);
                        break;
                    case EventType::Read:
                        {
                            string buf(16, '\0');
                            MSS(d.read(buf));
                            if (tty_ == d)
                            {
                                switch (mode_)
                                {
                                    case Raw:
                                        cout << buf.size() << " " << gubg::testing::toHex(buf) << endl;
                                        break;
                                    case Msgpack:
                                        factory_.process(buf);
                                        break;
                                    case D9_Msgpack:
                                        for (auto ch: buf)
                                            d9Decoder_.process(ch);
                                        break;
                                }
                            }
                            else
                            {
                                cout << "User typed: " << buf << endl;

                                gubg::msgpack::Serializer<std::string, garf::pod::TypeIds, 3> serializer;
                                garf::pod::Led led; led.id = 13; led.pattern = 0x7f;
                                serializer.serialize(led);

                                size_t nrWritten;
                                tty_.write(nrWritten, serializer.buffer());
                                cout << "I wrote " << nrWritten << " bytes: " << gubg::testing::toHex(serializer.buffer()) << endl;
                            }
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
            Mode mode_;
            Factory factory_;
            D9Decoder d9Decoder_;
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

        Select s;

        {
            OptionParser optionParser("Network and file monitor");
            optionParser.addSwitch("-h", "--help", "Displays this help", [&optionParser](){MONITOR_FINALIZE_OK(optionParser.help());});
            optionParser.addSwitch("-r", "--raw", "Raw output mode", [&s](){s.setMode(Raw);});
            optionParser.addSwitch("-m", "--msgpack", "Msgpack output mode", [&s](){s.setMode(Msgpack);});

            auto args = OptionParser::createArgs(argc, argv);
            MSS(optionParser.parse(args));
        }

        auto d = Descriptor::listen(File("/dev/ttyACM0"), AccessMode::ReadWrite);
        MSS(d.valid());
        s.add(d, AccessMode::Read);

        while (true)
        {
            s.process(std::chrono::milliseconds(500));
        }

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
