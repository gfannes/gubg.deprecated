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

            if (tv.tag == Tag("start"))
            {
                {
                    file::File f;
                    if (resolve_(f, tv.value))
                    {
                        MSS(processFileRef_(board, tv.value.as_file()));
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
            if (tv.tag == Tag("c++.source_ref"))
            {
                file::File f;
                MSS(resolve_(f, tv.value));
                const Tag tag("c++.source");
                board.setTypeForTag(tag, Type::File);
                board.addItem(tag, f);
            }
            if (tv.tag == Tag("c++.source"))
            {
                if (addExeChain_())
                {
                    AgentFactory fact;
                    MSS(board.addAgent(fact.createAgent("ExtractIncludes")));
                    MSS(board.addAgent(fact.createAgent("ParseInclude")));
                    MSS(board.addAgent(fact.createAgent("ResolveHeader")));
                    MSS(board.addAgent(fact.createAgent("Hasher")));
                    MSS(board.addAgent(fact.createAgent("Compiler")));
                    MSS(board.addAgent(fact.createAgent("Linker")));
                    MSS(board.addAgent(fact.createAgent("Runner")));
                }
            }
            if (tv.tag == Tag("chai.script_ref"))
            {
                file::File f;
                MSS(resolve_(f, tv.value));
                const Tag tag("chai.script");
                board.setTypeForTag(tag, Type::File);
                board.addItem(tag, f);
            }
            if (tv.tag == Tag("chai.script"))
            {
                AgentFactory fact;
                auto chai = fact.createChai(tv.value.as_file());
                MSS((bool)chai, ChaiScriptLoadFailed);
                MSS(board.addAgent(chai));
            }
        }

        MSS_END();
    }

    ReturnCode Starter::processFileRef_(Board &board, const file::File &ref)
    {
        MSS_BEGIN(ReturnCode);

        AgentFactory fact;

        if (false) {}
        else if (ref.extension() == "cpp")
        {
            board.addItem(Tag("c++.source_ref"), ref);
        }
        else if (ref.extension() == "chai")
        {
            board.addItem(Tag("chai.script_ref"), ref);
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
        else if (str == "ig")
        {
            MSS(board.addAgent(fact.createAgent("FixIncludeGuard")));
        }
        else if (str == "IG")
        {
            MSS(board.addAgent(fact.createAgent("FixIncludeGuard")));
            MSS(board.addItem(Tag{"ig.fix"}, Value{"true"}));
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

            board.addItem(Tag(key_parts), value.str());
        }
        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
