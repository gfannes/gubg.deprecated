#include <iostream>
using namespace std;
//ostream &operator<<(ostream &os, const pair<string, bool> &t){return os << "(" << t.first << ", " << t.second << ")";}
//ostream &operator<<(ostream &os, const pair<string, string> &t){return os << "(" << t.first << ", " << t.second << ")";}
template <typename T>
ostream &insertToStream(ostream &os, const T &t)
{
    for (auto it = t.begin(); it != t.end(); ++it)
    {
        os << *it << ", ";
    }
    return os;
}
template <typename T>
ostream &insertToStream2(ostream &os, const T &t)
{
    for (auto it = t.begin(); it != t.end(); ++it)
    {
        os << "(" << it->first << ", " << it->second << ")" << ", ";
    }
    return os;
}
ostream &operator<<(ostream &os, const OptionParser::Args &t){return insertToStream(os, t);}
ostream &operator<<(ostream &os, const map<string, bool> &t){return insertToStream2(os, t);}
ostream &operator<<(ostream &os, const map<string, string> &t){return insertToStream2(os, t);}
int main(int argc, char **argv)
{
    const string version("1.2.3");

    OptionParser parser("OptionParser demo");
    map<string, bool> switches;
    switches["verbose"] = false;
    parser.addSwitch("-v", "--verbose", "Verbose", [&switches](){switches["verbose"] = true;});
    parser.addSwitch("-V", "--version", "Print version", [version](){cout << "Version: " << version << endl;});
    map<string, string> options;
    options["output"] = "stdout";
    parser.addOptional("-o", "--output FILE", "Output FILE (default stdout)", [&options](string option){if (!option.empty()) options["output"] = option;});
    parser.addMandatory("-i", "--input FILE", "Input FILE", [&options](string input){options["input"] = input;});

    cout << parser.help();

    OptionParser::Args args = OptionParser::convertArgs(argc, argv);
    cout << "Before parsing the args: " << args << endl;
    auto res = parser.parse(args);
    cout << "Parsing was " << (res ? "OK" : "not OK") << endl;
    cout << "After parsing the args: " << args << endl;
    cout << "Switches: " << switches << endl;
    cout << "Options: " << options << endl;

    return 0;
}
