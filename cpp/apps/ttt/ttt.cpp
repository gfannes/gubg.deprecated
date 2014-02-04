//!@#adoc
//Trace tracking tool
#include "ttt/Codes.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Descriptor.hpp"
#include <iostream>
using namespace std;
using namespace gubg;
using namespace ttt;

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
} 

#define GUBG_MODULE_ "ttt"
#include "gubg/log/begin.hpp"
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
