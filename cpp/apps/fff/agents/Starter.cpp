#include "fff/agents/Starter.hpp"
#include "fff/AgentFactory.hpp"
#include "fff/Tags.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
#include "gubg/Strange.hpp"
using namespace gubg;
using namespace gubg::bbs;

#define GUBG_MODULE_ "Starter"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

	bool resolve_(file::File &f, const Value &v)
	{
		file::File tmp(v);
		if (!MSS_IS_OK(file::resolve(tmp)))
			return false;
		f = tmp;
		return true;
	}

    gubg::bbs::ReturnCode Starter::process(Board &board)
    {
        MSS_BEGIN(gubg::bbs::ReturnCode);

        auto tvs = board.getFrom(ix_);
        ix_ += tvs.size();

        for (auto tv: tvs)
        {
            SS(tv.tag, tv.value);

            if (tv.tag == start)
            {
                {
                    file::File f;
                    if (resolve_(f, tv.value))
                    {
                        MSS(processFileRef_(board, tv.value));
                        continue;
                    }
                }
                {
                    if (MSS_IS_OK(processCommand_(board, tv.value)))
                        continue;
                }
                MSS(processOption_(board, tv.value));
            }
            if (tv.tag == cpp_source_ref)
            {
                file::File f;
                MSS(resolve_(f, tv.value));
                board.addItem(cpp_source, f);
            }
            if (tv.tag == cpp_source)
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
            if (tv.tag == "chai.script_ref")
            {
                file::File f;
                MSS(resolve_(f, tv.value));
                board.addItem("chai.script", f);
            }
            if (tv.tag == "chai.script")
            {
                AgentFactory fact;
                auto chai = fact.createChai(tv.value);
                MSS((bool)chai);
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
            board.addItem(cpp_source_ref, ref);
        }
        else if (ref.extension() == "chai")
        {
            board.addItem("chai.script_ref", ref);
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
            MSS(board.addItem(ig_fix, "true"));
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

            board.addItem(key_parts, value.str());
        }
        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
