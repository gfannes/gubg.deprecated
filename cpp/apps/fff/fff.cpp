#include "fff/Codes.hpp"
#include "fff/Options.hpp"
#include "fff/Board.hpp"
#include "fff/ToolFactory.hpp"
#include "gubg/OptionParser.hpp"
#include <algorithm>

#define GUBG_MODULE_ "fff"
#include "gubg/log/begin.hpp"
namespace fff { 

	std::string g_help;
	void showHelp()
	{
		std::cout << g_help;
		std::cout << "Geert Fannes" << std::endl;
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

		Board board;
		MSS(board.add(Tag("cache"), gubg::file::File("/tmp")));
		for (auto seed: options.seeds)
			MSS(board.add(Tag("start"), seed));

		ToolFactory fact;
		MSS(board.addTool(fact.createTool("Starter")));

        board.expand();

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"

int main(int argc, char **argv)
{
	switch (fff::main(argc, argv))
	{
		case fff::ReturnCode::OK:
			break;
		case fff::ReturnCode::NoSeedGiven:
			std::cerr << "You have to give me a seed to start the process" << std::endl;
			return -1;
			break;
		default:
			return -1;
			break;
	}
	return 0;
}
