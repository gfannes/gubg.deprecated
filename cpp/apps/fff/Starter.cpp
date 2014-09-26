#include "fff/Starter.hpp"
#include "fff/Value.hpp"
#include "fff/Board.hpp"
#include "fff/ToolFactory.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
using namespace gubg;

#define GUBG_MODULE_ "Starter"
#include "gubg/log/begin.hpp"
namespace fff { 
	bool resolve_(file::File &f, const Value &v)
	{
		file::File tmp(v.string());
		if (!MSS_IS_OK(file::resolve(tmp)))
			return false;
		f = tmp;
		return true;
	}
	ReturnCode Starter::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		auto tvs = board.getFrom(ix_);
		ix_ += tvs.size();

		ToolFactory fact;
		for (auto tv: tvs)
		{
			SS(tv.first, tv.second);
			if (tv.first != Tag("start"))
				continue;
			file::File f;
			if (!resolve_(f, tv.second))
			{
				L("Could not resolve "<<tv.second);
				continue;
			}
			if (false) {}
			else if (f.extension() == "cpp")
			{
				if (addExeChain_())
				{
					MSS(board.addTool(fact.createTool("ParseIncludes")));
					MSS(board.addTool(fact.createTool("ResolveHeader")));
					MSS(board.addTool(fact.createTool("Compiler")));
					MSS(board.addTool(fact.createTool("Linker")));
					MSS(board.addTool(fact.createTool("Runner")));
					board.add(Tag("c++", "source"), f);
				}
			}
			else if (f.extension() == "lua")
			{
				std::string code;
				MSS(file::read(code, f));
				std::string err;
				switch (const auto rc = board.executeLua(code, err))
				{
					default:
						std::cout << "Lua error encountered:" << std::endl;
						for (auto line: gubg::line::split(err))
							std::cout << line.txt << std::endl;
					case ReturnCode::OK:
						MSS(rc);
				}
			}
		}

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
