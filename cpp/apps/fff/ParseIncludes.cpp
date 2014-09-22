#include "fff/ParseIncludes.hpp"
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
namespace fff { 
	ReturnCode ParseIncludes::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		auto tvs = board.getFrom(ix_);
		MSS(!tvs.empty(), NoNewEntries);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (tv.first != Tag("c++", "source") && tv.first != Tag("c++", "header"))
				continue;
			MSS(tv.second.type() == Value::File);
			IncludeParser ip;
			if (!MSS_IS_OK(ip.process(tv.second.file())))
				std::cerr << "Failed to parse include statements for " << tv.second.file() << std::endl;
			else
				for (auto inc: ip.includes)
					board.add(Tag("c++", "include"), inc, tv);
		}

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
