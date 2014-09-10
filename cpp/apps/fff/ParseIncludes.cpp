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
			{
				string content;
				//Could be more efficient, the include parser will read the file again hereunder...
				MSS(file::read(content, tv.second.file()));
				gubg::hash::MD5 md5; md5 << content;
				board.setHash(tv, md5.hash());
			}
			IncludeParser ip;
			MSS(ip.process(tv.second.file()));
			for (auto inc: ip.includes)
				board.add(Tag("c++", "include"), inc, tv);
		}

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
