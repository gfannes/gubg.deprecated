#include "da/Codes.hpp"
#include "da/Tasks.hpp"
#include "gubg/l.hpp"
#include "gubg/OptionParser.hpp"
#include <cstdlib>
using namespace da;
using gubg::OptionParser;

namespace 
{
    template <typename OP>
        void showHelpAndQuit(const OP &op)
        {
            L(op.help());
            std::exit(0);
        }
    ReturnCode main_(int argc, char **argv)
    {
        MSS_BEGIN(ReturnCode);
        OptionParser::Args args;
        MSS(OptionParser::createArgs(args, argc, argv));
        OptionParser optionParser("Develop assistent");
        optionParser.addSwitch("-h", "--help", "Displays this help", [&optionParser](){showHelpAndQuit(optionParser);});

        Tasks tasks;
        optionParser.addSwitch("-g", "--guards", "Fix include guards", [&tasks](){tasks.push_back(FixIncludeGuards::create());});
        MSS(optionParser.parse(args));
        for (auto task: tasks)
            MSS(task->execute());
        MSS_END();
    }
}

int main(int argc, char **argv)
{
    if (!gubg::mss::isOK(main_(argc, argv)))
    {
        L("ERROR");
        return -1;
    }
    L("Everything went OK");
    return 0;
}
