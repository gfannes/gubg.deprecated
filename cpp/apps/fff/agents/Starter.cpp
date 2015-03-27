#include "fff/agents/Starter.hpp"
#include "fff/Value.hpp"
#include "fff/Board.hpp"
#include "fff/AgentFactory.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
#include "gubg/Strange.hpp"
using namespace gubg;

#define GUBG_MODULE_ "Starter"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

	bool resolve_(file::File &f, const Value &v)
	{
		file::File tmp(v.as_string());
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

        for (auto tv: tvs)
        {
            SS(tv.tag, tv.value);

            if (tv.tag != Tag("start"))
                //We only handle tvs added from CLI arguments
                continue;

            {
                file::File f;
                if (resolve_(f, tv.value))
                {
                    MSS(processFile_(board, f));
                    continue;
                }
            }

            {
                const std::string str = tv.value.as_string();
                if (MSS_IS_OK(processCommand_(board, str)))
                    continue;
            }

            MSS(processOption_(board, tv.value.as_string()));
        }

        MSS_END();
    }

    ReturnCode Starter::processFile_(Board &board, const file::File &f)
    {
        MSS_BEGIN(ReturnCode);

        AgentFactory fact;

        if (false) {}
        else if (f.extension() == "cpp")
        {
            if (addExeChain_())
            {
                MSS(board.addAgent(fact.createAgent("ParseIncludes")));
                MSS(board.addAgent(fact.createAgent("ResolveHeader")));
                MSS(board.addAgent(fact.createAgent("Compiler")));
                MSS(board.addAgent(fact.createAgent("Linker")));
                MSS(board.addAgent(fact.createAgent("Runner")));
                board.add(Tag("c++", "source"), f);
            }
        }

        MSS_END();
    }
    ReturnCode Starter::processCommand_(Board &board, const std::string &str)
    {
        MSS_BEGIN(ReturnCode);

        AgentFactory fact;

        if (false) {}
        else if (str == "search")
        {
            MSS(board.addAgent(fact.createAgent("Search")));
        }
        else
            MSS_QL(UnknownCommand);

        MSS_END();
    }
    ReturnCode Starter::processOption_(Board &board, const std::string &str)
    {
        MSS_BEGIN(ReturnCode);
        Strange strange(str);
        Strange key;
        if (strange.popUntil(key, ':'))
        {
            //Split the key on '.'
            std::vector<std::string> key_parts;
            {
                Strange part;
                while (key.popUntil(part, '.'))
                    key_parts.push_back(part.str());
                key.popAll(part);
                key_parts.push_back(part.str());
            }

            //Value is the part after the ':'
            Strange value;
            strange.popAll(value);

            board.add(Tag(key_parts), value.str());
        }
        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
