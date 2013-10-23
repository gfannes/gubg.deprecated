#include "pa/Codes.hpp"
#include "pa/Tasks.hpp"
#include "pa/Finalize.hpp"
#include "gubg/OptionParser.hpp"
using namespace pa;
using gubg::OptionParser;
using namespace std;

#define GUBG_MODULE_ "pa"
#include "gubg/log/begin.hpp"
namespace 
{
    ReturnCode main_(int argc, char **argv)
    {
        MSS_BEGIN(ReturnCode);

        Tasks tasks;
        Options options;
		{
			bool loadMindMap = false;
			OptionParser optionParser("Develop assistent");
			optionParser.addSwitch("-h", "--help", "Displays this help", [&optionParser](){PA_FINALIZE_OK(optionParser.help());});
			optionParser.addMandatory("-i", "--input FILE", "Input file to use", [&options](string file){options.input = file;});
			optionParser.addMandatory("-o", "--output FILE", "Output file to use", [&options](string file){options.output = file;});
			optionParser.addMandatory("-f", "--fraction NAME", "Use freeplane attribute NAME as fraction", [&options](string name){options.fraction = name;});
			optionParser.addMandatory("-F", "--fraction-default VALUE", "Default fraction to use", [&options](string value){options.fraction_default = value;});
			optionParser.addMandatory("-l", "--line NAME", "Line NAME to use", [&options](string name){options.lines.push_back(name);});
			optionParser.addMandatory("-t", "--totals NAME", "The totals to be used",
					[&options, &tasks, &loadMindMap](string name){options.value = name; loadMindMap = true;});
			optionParser.addMandatory("-c", "--category NAME", "The category to be used for quarter reporting",
					[&options, &tasks, &loadMindMap](string name){options.category = name; loadMindMap = true;});
			optionParser.addSwitch("-d", "--details", "Show detailed work breakdown",
					[&options, &tasks]()
					{
					if (options.value.empty()) PA_FINALIZE_OK("You have to specify the totals to be used before the details");
					tasks.push_back(ShowWBS::create());
					});
			optionParser.addSwitch("-p", "--plan-overview", "Planning overview",
					[&options, &tasks]()
					{
					if (options.value.empty()) PA_FINALIZE_OK("You have to specify the totals to be used before the planning overview");
					tasks.push_back(Plan::create(Plan::Overview));
					});
			optionParser.addSwitch("-P", "--plan-details", "Planning details",
					[&options, &tasks]()
					{
					if (options.value.empty()) PA_FINALIZE_OK("You have to specify the totals to be used before the planning details");
					tasks.push_back(Plan::create(Plan::Details));
					});
			optionParser.addSwitch("-q", "--quarter", "Quarter view of the planning",
					[&options, &tasks]()
					{
					if (options.value.empty()) PA_FINALIZE_OK("You have to specify the totals to be used before the quarter view");
					if (options.category.empty()) PA_FINALIZE_OK("You have to specify the category to be used before the quarter view");
					tasks.push_back(Quarter::create());
					});
			optionParser.addSwitch("-D", "--debug", "Debug view of the tree",
					[&options, &tasks]()
					{
					if (options.value.empty()) PA_FINALIZE_OK("You have to specify the totals to be used before the debug view");
					tasks.push_back(ShowDebug::create());
					});

			OptionParser::Args args;
			MSS(OptionParser::createArgs(args, argc, argv));
			MSS(optionParser.parse(args));
			if (loadMindMap)
				tasks.push_front(LoadMindMap::create());
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
