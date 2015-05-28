#include "fff/agents/ExtractIncludes.hpp"
#include "fff/Tags.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#include <list>
#include <string>
#include <cassert>
using namespace gubg;
using namespace std;

#define GUBG_MODULE "ExtractIncludes"
#include "gubg/log/begin.hpp"
namespace {
    struct IncludeParser: gubg::parse::cpp::Includes_crtp<IncludeParser>
    {
        list<string> includes;
        template <typename String, typename Type>
            void includes_detected(const String &str, Type type)
            {
                assert(!str.empty());
                includes.push_back(str);
            }
    };
}
namespace fff { namespace agents { 
	gubg::bbs::ReturnCode ExtractIncludes::process(gubg::bbs::Board &board)
	{
		MSS_BEGIN(gubg::bbs::ReturnCode);

		auto tvs = board.getFrom(ix_);
		MSS(!tvs.empty(), NoNewEntries);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (tv.tag != cpp_source && tv.tag != cpp_header)
				continue;
			IncludeParser ip;
			if (!MSS_IS_OK(ip.process(tv.value)))
				std::cerr << "Failed to parse include statements for " << tv.value << std::endl;
			else
				for (auto inc: ip.includes)
					board.addItem("c++.include", inc, tv);
		}

		MSS_END();
	}
} } 
#include "gubg/log/end.hpp"
