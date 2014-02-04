//!@#adoc
//Trace tracking tool
#include "ttt/Codes.hpp"
#include "gubg/OptionParser.hpp"
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
} 

#define GUBG_MODULE "ttt"
#include "gubg/log/begin.hpp"
ReturnCode main_(int argc, char **argv)
{
    MSS_BEGIN(ReturnCode);
    OptionParser options("trace tracking tool");
    options.addSwitch("-h", "--help", "Displays this help", [&](){finalize(options.help(), 0);});
    auto args = OptionParser::createArgs(argc, argv);
    options.parse(args);
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
