#include "pa/Codes.hpp"
#include "pa/Tasks.hpp"
#include "pa/Finalize.hpp"
#include "gubg/OptionParser.hpp"
using namespace pa;
using gubg::OptionParser;
using namespace std;

#define GUBG_MODULE "pa"
#include "gubg/log/begin.hpp"
namespace 
{
    ReturnCode main_(int argc, char **argv)
    {
        MSS_BEGIN(ReturnCode);

        Tasks tasks;
        Options options;
        {
            OptionParser optionParser("Develop assistent");
            optionParser.addSwitch("-h", "--help", "Displays this help", [&optionParser](){PA_FINALIZE_OK(optionParser.help());});
            optionParser.addMandatory("-e", "--extract FILE", "Extract planning from a freeplane mindmap",
                    [&tasks](string file){tasks.push_back(ExtractPlanning::create(file));});

            OptionParser::Args args;
            MSS(OptionParser::createArgs(args, argc, argv));
            MSS(optionParser.parse(args));
        }

        for (auto task: tasks)
            MSS(task->execute(options));

        MSS_END();
    }
}

int main(int argc, char **argv)
{
    if (!gubg::mss::isOK(main_(argc, argv)))
        PA_FINALIZE_ERROR("Problems were encountered...");
    PA_FINALIZE_OK("Everything went OK");
    return 0;
}
