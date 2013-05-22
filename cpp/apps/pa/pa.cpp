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
            optionParser.addMandatory("-i", "--input FILE", "Input file to use", [&options](string file){options.input = file;});
            optionParser.addMandatory("-o", "--output FILE", "Output file to use", [&options](string file){options.output = file;});
            optionParser.addMandatory("-f", "--fraction NAME", "Use freeplane attribute NAME as fraction", [&options](string name){options.fraction = name;});
            optionParser.addMandatory("-F", "--fraction-default VALUE", "Default fraction to use", [&options](string value){options.fraction_default = value;});
            optionParser.addMandatory("-t", "--totals NAME", "The totals to be used",
                    [&options, &tasks](string name){options.value = name; tasks.push_back(LoadMindMap::create());});
            optionParser.addSwitch("-d", "--details", "Show detailed work breakdown",
                    [&options, &tasks]()
                    {
                    if (options.value.empty()) PA_FINALIZE_OK("You have to specify the totals to be used");
                    tasks.push_back(ShowWBS::create());
                    });
            optionParser.addSwitch("-p", "--plan", "Basic planning",
                    [&options, &tasks]()
                    {
                    if (options.value.empty()) PA_FINALIZE_OK("You have to specify the totals to be used");
                    tasks.push_back(Plan::create());
                    });

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
