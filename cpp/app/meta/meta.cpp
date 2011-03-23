#include "Exit.hpp"
#include "gubg/OptionParser.hpp"
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
    if (!parser.parse(args))
        meta::finalize("Failed to parse the arguments");

    cout << "verbose: " << switches["verbose"]  << endl;
    return 0;
}
