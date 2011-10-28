#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

namespace
{
    enum class ReturnCode {MSS_DEFAULT_CODES};
    const string version("0.1");
}
int main(int argc, char **argv)
{
    MSS_BEGIN(ReturnCode);

    OptionParser parser("Generic build utility");
    parser.addSwitch("-V", "--version", "Print version", [version](){cout << "Version: " << version << endl;});
    parser.addSwitch("-h", "--help", "Print this help", [&parser](){cout << parser.help() << endl;});

    OptionParser::Args args;
    MSS_T(OptionParser::createArgs(args, argc, argv), IllegalArgument);

    MSS_T(parser.parse(args), IllegalArgument);

    MSS_FAIL();
    return -1;

    MSS_END();
    return 0;
}
