#include "fff/agents/ParseIncludes.hpp"
#include "fff/Board.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#include <list>
#include <string>
#include <cassert>
using namespace gubg;
using namespace std;

#define GUBG_MODULE "ParseIncludes"
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
	ReturnCode ParseIncludes::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		auto tvs = board.getFrom(ix_);
		MSS(!tvs.empty(), NoNewEntries);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (tv.tag != Tag("c++", "source") && tv.tag != Tag("c++", "header"))
				continue;
			IncludeParser ip;
			if (!MSS_IS_OK(ip.process(tv.value.as_file())))
				std::cerr << "Failed to parse include statements for " << tv.value.as_file() << std::endl;
			else
				for (auto inc: ip.includes)
					board.addItem(Tag("c++", "include"), inc, tv);
		}

		MSS_END();
	}
} } 
#include "gubg/log/end.hpp"
