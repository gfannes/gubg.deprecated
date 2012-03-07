#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"
using namespace gubg;
using namespace std;

MSS_CODES_BEGIN(OptionParser::ReturnCode);
MSS_CODE(CouldNotFindExecutable);
MSS_CODE(ExpectedMandatoryArgument);
MSS_CODES_END();

OptionParser::OptionParser(const string &caption):
    caption_(caption){}

OptionParser::ReturnCode OptionParser::createArgs(Args &args, int argc, char **argv)
{
    MSS_BEGIN(ReturnCode);
    MSS_T(argc >= 0, IllegalArgument);
    MSS_T(argv, IllegalArgument);
    for (int i = 0; i < argc; ++i)
        args.push_back(argv[i]);
    MSS_END();
}

OptionParser::ReturnCode OptionParser::parse(Args &args, bool stripExe)
{
    MSS_BEGIN(ReturnCode);
    if (stripExe)
    {
        MSS_T(!args.empty(), CouldNotFindExecutable);
        args.pop_front();
    }

    //Pop arguments one by one and look for corresponding callbacks
    while (!args.empty())
    {
        auto arg = args.front();
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
                MSS_L(ExpectedMandatoryArgument);
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
    MSS_END();
}

void OptionParser::addHelpLine_(const string &shortHand, const string &longHand, const string &description)
{
    help_ << shortHand << "\t" << longHand << "\t" << description << endl;
}
string OptionParser::help() const
{
    return caption_ + "\n" + help_.str();
}

#ifdef UnitTest
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
#endif
