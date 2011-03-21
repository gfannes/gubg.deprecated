#include "gubg/OptionParser.h"
#include <map>
#include <iostream>
using namespace std;
using namespace gubg;

int main(int argc, char **argv)
{
    OptionParser parser("Meta data compilation tool");
    map<string, bool> switches;
    switches["verbose"] = false;
    parser.addSwitch("-v", "--verbose", "Verbose mode", [&switches](){switches["verbose"] = true;});
    auto args = OptionParser::convertArgs(argc, argv);
    parser.parse(args);

    cout << "verbose: " << switches["verbose"]  << endl;
    return 0;
}
