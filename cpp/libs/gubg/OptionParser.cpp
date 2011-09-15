#include "gubg/OptionParser.hpp"
using namespace gubg;

OptionParser::OptionParser(const std::string &caption):
    caption_(caption){}

OptionParser::Args OptionParser::convertArgs(int argc, char **argv)
{
    Args args;
    for (int i = 0; i < argc; ++i)
        args.push_back(argv[i]);
    return args;
}

bool OptionParser::parse(Args &args, bool stripExe)
{
    if (stripExe)
    {
        if (args.empty())
            return false;
        args.pop_front();
    }

    //When args is empty, we are certainly done
    while (!args.empty())
    {
        std::string arg = args.front();
        args.pop_front();

        VoidCallbacks::const_iterator itVoid;
        StringCallbacks::const_iterator itString;
        if ((itVoid = switchCallbacks_.find(arg)) != switchCallbacks_.end())
            (itVoid->second)();
        else if ((itString = mandatoryCallbacks_.find(arg)) != mandatoryCallbacks_.end())
        {
            if (args.empty())
            {
                args.push_front(arg);
                return false;
            }
            (itString->second)(args.front());
            args.pop_front();
        }
        else if ((itString = optionalCallbacks_.find(arg)) != optionalCallbacks_.end())
        {
            if (args.empty() || args.front().empty() || '-' == args.front()[0])
                (itString->second)("");
            else
            {
                (itString->second)(args.front());
                args.pop_front();
            }
        }
        else
        {
            args.push_front(arg);
            break;
        }
    }
    return true;
}

void OptionParser::addHelpLine_(const std::string &shortHand, const std::string &longHand, const std::string &description)
{
    help_ << shortHand << "\t" << longHand << "\t" << description << std::endl;
}
std::string OptionParser::help() const
{
    return caption_ + "\n" + help_.str();
}

#ifdef UnitTest
#include <iostream>
using namespace std;
//std::ostream &operator<<(std::ostream &os, const pair<string, bool> &t){return os << "(" << t.first << ", " << t.second << ")";}
//std::ostream &operator<<(std::ostream &os, const pair<string, string> &t){return os << "(" << t.first << ", " << t.second << ")";}
template <typename T>
std::ostream &insertToStream(std::ostream &os, const T &t)
{
    for (auto it = t.begin(); it != t.end(); ++it)
    {
        os << *it << ", ";
    }
    return os;
}
template <typename T>
std::ostream &insertToStream2(std::ostream &os, const T &t)
{
    for (auto it = t.begin(); it != t.end(); ++it)
    {
        os << "(" << it->first << ", " << it->second << ")" << ", ";
    }
    return os;
}
std::ostream &operator<<(std::ostream &os, const OptionParser::Args &t){return insertToStream(os, t);}
std::ostream &operator<<(std::ostream &os, const map<string, bool> &t){return insertToStream2(os, t);}
std::ostream &operator<<(std::ostream &os, const map<string, string> &t){return insertToStream2(os, t);}
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
#endif
