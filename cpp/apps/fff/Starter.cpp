#include "fff/Starter.hpp"
#include "fff/Value.hpp"
#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace gubg;

#define GUBG_MODULE "Starter"
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
		for (auto tv: tvs)
		{
			if (tv.first != Tag("start"))
				continue;
			file::File f;
			if (resolve_(f, tv.second))
			{
				if (false) {}
				else if (f.extension() == "cpp")
				{
					board.add(Tag("c++", "source"), f);
					started_ = true;
					break;
				}
			}
		}
		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
