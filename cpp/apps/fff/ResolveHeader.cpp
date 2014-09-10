#include "fff/ResolveHeader.hpp"
#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Variables.hpp"
#include <regex>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "ResolveHeader"
#include "gubg/log/begin.hpp"
namespace fff { 
	bool gubg_home_(file::File &f)
	{
		string v;
		if (!env::Variables::shell().get(v, "GUBG"))
			return false;
		f = v;
		return true;
	}
	ResolveHeader::ResolveHeader()
	{
		file::File f;
		if (gubg_home_(f))
			gubg_.reset(new file::File(f << "cpp/libs/gubg"));
	}
	ReturnCode ResolveHeader::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		auto tvs = board.getFrom(ix_);
		MSS(!tvs.empty(), NoNewEntries);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.first == Tag("start"))
			{
				file::File path(tv.second.string());
				if (MSS_IS_OK(file::resolve(path)) and path.popBasename())
				{
					L("Adding local (" << path << ")");
					forest_.add(path, {"hpp", "cpp"});
					{
						auto ip = path; ip.popBasename();
						board.add(Tag("c++", "include_path"), ip, tv);
					}
				}
			}
			else if (tv.first == Tag("c++", "include"))
			{
				MSS(tv.second.type() == Value::String);

				if (gubg_ and roots_.count(*gubg_) == 0)
				{
					static const regex gubg_re("gubg/.+\\.hpp");
					if (regex_match(tv.second.string(), gubg_re))
					{
						L("Adding gubg (" << *gubg_ << ")");
						forest_.add(*gubg_, {"hpp", "cpp"});
						roots_.insert(*gubg_);
						{
							auto ip = *gubg_; ip.popBasename();
							board.add(Tag("c++", "include_path"), ip, tv);
						}
					}
				}

				file::File rf;
				if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.second.string()))))
					board.add(Tag("c++", "header"), rf);
			}
			else if (tv.first == Tag("c++", "header"))
			{
				const auto header = tv.second.file();
				file::File source = header; source.setExtension("cpp");
				if (forest_.contains(source))
					board.add(Tag("c++", "source"), source, tv);
			}
		}

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
