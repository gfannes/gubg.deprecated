#include "fff/Codes.hpp"
#include "fff/Options.hpp"
#include "fff/AgentFactory.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/chrono/Stopwatch.hpp"
#include "gubg/env/Variables.hpp"
#include <algorithm>
#include <iomanip>

#include <iostream>

#define GUBG_MODULE_ "fff"
#include "gubg/log/begin.hpp"
namespace fff { 

	std::string g_help;
	void showHelp()
	{
		std::cout << g_help;
		std::cout << "Geert Fannes (" << __DATE__ << " " << __TIME__ << ")" << std::endl;
	}

	ReturnCode parseOptions(Options &options, int argc, char **argv)
	{
		MSS_BEGIN(ReturnCode);
		gubg::OptionParser parser("flexible file factory: fff [options] <seeds>");
		parser.addSwitch("-h", "--help", "Show help", [&options, &parser](){options.showHelp = true; g_help = parser.help();});
		auto args = gubg::OptionParser::createArgs(argc, argv);
		parser.parse(args);

		options.seeds.resize(args.size());
		std::copy(args.begin(), args.end(), options.seeds.begin());

		MSS_END();
	}

	ReturnCode main(int argc, char **argv)
	{
		MSS_BEGIN(ReturnCode);

		Options options;
		MSS(parseOptions(options, argc, argv));
		
		if (options.showHelp)
		{
			showHelp();
			MSS_RETURN_OK();
		}

		MSS(!options.seeds.empty(), NoSeedGiven);

        gubg::bbs::Board board;

		auto cache = gubg::file::File("/tmp/fff");
        {
            std::string str;
            if (gubg::env::Variables::shell().get(str, "FFF_CACHE"))
            {
                gubg::file::File dir = str;
                gubg::file::mkdir(dir);
                if (gubg::file::isDirectory(dir))
                    cache = dir;
            }
        }
		if (!gubg::file::isDirectory(cache))
			std::cout << "Cache directory (" << cache << ") not found, working without cache" << std::endl;
		else
			MSS(board.addItem("cache", cache));

		for (auto seed: options.seeds)
        {
            const auto rc = board.addItem("start", seed);
			MSS(rc);
        }

		AgentFactory fact;
		MSS(board.addAgent(fact.createAgent("Starter")));

		MSS(board.expand());

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"

int main(int argc, char **argv)
{
	gubg::chrono::Stopwatch<> sw;
	switch (const auto rc = fff::main(argc, argv))
	{
		case fff::ReturnCode::OK:
            std::cout << "Everything went OK (" << sw.mark().total_elapse_hr() << " seconds)" << std::endl;
			break;
		case fff::ReturnCode::NoSeedGiven:
			std::cerr << "You have to give me a seed to start the process" << std::endl;
			return -(int)rc;
			break;
		default:
			std::cerr << "ReturnCode " << (int)rc << std::endl;
			return -(int)rc;
			break;
	}
	return 0;
}
