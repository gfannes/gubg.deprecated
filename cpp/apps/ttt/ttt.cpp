//!@#adoc
//Trace tracking tool
#include "ttt/Codes.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Descriptor.hpp"
#include <iostream>
using namespace std;
using namespace gubg;
using namespace ttt;

#define GUBG_MODULE_ "ttt"
#include "gubg/log/begin.hpp"
namespace ttt { 
    void finalize(const string &msg, int ret)
    {
        cout << msg << endl;
        exit(ret);
    }
    struct Options
    {
        unsigned short port = 1234;
    };
    std::ostream &operator<<(std::ostream &os, const Options &options)
    {
        os << "Port: " << options.port << endl;
        return os;
    }
    class Select: public file::Select
    {
        public:
            virtual void select_ready(file::Descriptor d, file::EventType et)
            {
                S();L(d << " is ready for some action: " << to_hr(et));
                switch (et)
                {
                    case file::EventType::Open:
                        {
                            file::Descriptor c;
                            d.accept(c);
                            add(c, file::AccessMode::Read);
                        }
                        break;
                    case file::EventType::Read:
                        {
                            std::string str(1024, '?');
                            d.read(str);
                            L(str);
                        }
                        break;
                }
            }
        private:
    };
} 

ReturnCode main_(int argc, char **argv)
{
    MSS_BEGIN(ReturnCode);
    Options options;
    {
        OptionParser parser("trace tracking tool");
        parser.addSwitch("-h", "--help", "Displays this help", [&](){finalize(parser.help(), 0);});
        parser.addMandatory("-p", "--port", "TCP port to listen", [&](const string &port){options.port = atoi(port.c_str());});
        auto args = OptionParser::createArgs(argc, argv);
        parser.parse(args);
    }
    L(options);

    auto d = file::Descriptor::listen(1234);
    MSS(d.valid());
    Select s;
    s.add(d, file::AccessMode::Read);
    while (true)
    {
        S();
        s.process(std::chrono::seconds(60));
    }

    MSS_END();
}
int main(int argc, char **argv)
{
    const auto rc = main_(argc, argv);
    switch (rc)
    {
    }
    cout << "Everthing went OK" << endl;
    return 0;
}
#include "gubg/log/end.hpp"
