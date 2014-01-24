#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE "OptionParser"
#include "gubg/log/begin.hpp"
MSS_CODES_BEGIN(OptionParser::ReturnCode);
MSS_CODE(CouldNotFindExecutable);
MSS_CODE(ExpectedMandatoryArgument);
MSS_CODES_END();

OptionParser::OptionParser(const string &caption):
    caption_(caption){}

OptionParser::Args OptionParser::createArgs(int argc, char **argv)
{
	Args args;
	if (argv)
	{
		for (int i = 0; i < argc; ++i)
			args.push_back(argv[i]);
	}
	return args;
}

OptionParser::ReturnCode OptionParser::parse(Args &args, bool stripExe)
{
    MSS_BEGIN(ReturnCode);
    if (stripExe)
    {
        MSS(!args.empty(), CouldNotFindExecutable);
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
