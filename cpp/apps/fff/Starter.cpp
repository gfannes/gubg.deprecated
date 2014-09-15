#include "fff/Starter.hpp"
#include "fff/Value.hpp"
#include "fff/Board.hpp"
#include "fff/ToolFactory.hpp"
#include "gubg/file/Filesystem.hpp"
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
		MSS_Q(!started_, AlreadyStarted);
		auto tvs = board.getFrom(0);
		ToolFactory fact;
		for (auto tv: tvs)
		{
			SS(STREAM(tv.first, tv.second));
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
				MSS(board.addTool(fact.createTool("ParseIncludes")));
				MSS(board.addTool(fact.createTool("ResolveHeader")));
				MSS(board.addTool(fact.createTool("Compiler")));
				MSS(board.addTool(fact.createTool("Linker")));
				board.add(Tag("c++", "source"), f);
				started_ = true;
			}
			else if (f.extension() == "lua")
			{
				std::string code;
				MSS(file::read(code, f));
				MSS(board.executeLua(code));
				started_ = true;
			}
		}
		MSS(started_);
		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
