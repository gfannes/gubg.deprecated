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
	bool gubg_env_var_(file::File &f, const string &var)
	{
		string v;
		if (!env::Variables::shell().get(v, var))
			return false;
		f = v;
		return true;
	}
	bool gubg_home_(file::File &f)
	{
        return gubg_env_var_(f, "GUBG");
	}
	bool gubg_sdks_(file::File &f)
	{
        return gubg_env_var_(f, "GUBG_SDKS");
	}
	ResolveHeader::ResolveHeader()
	{
		file::File f;
		if (gubg_home_(f))
			gubg_.reset(new file::File(f << "cpp/libs/gubg"));
		if (gubg_home_(f))
			lua_.reset(new file::File(f << "c/lua-5.2.3"));
		if (gubg_home_(f))
			catch_.reset(new file::File(f << "cpp/libs/catch"));
		if (gubg_sdks_(f))
			sfml_.reset(new file::File(f << "SFML"));
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
					forest_.add(path, {"hpp", "cpp", "h", "c"});
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

				if (catch_ and roots_.count(*catch_) == 0)
				{
					if (tv.second.string() == "catch/catch.hpp")
					{
						L("Adding catch (" << *catch_ << ")");
						forest_.add(*catch_, {"hpp", "cpp"});
						roots_.insert(*catch_);
						{
							auto ip = *catch_; ip.popBasename();
							board.add(Tag("c++", "include_path"), ip, tv);
						}
					}
				}

				if (lua_ and roots_.count(*lua_) == 0)
				{
					static const regex lua_re("lua.hpp");
					if (regex_match(tv.second.string(), lua_re))
					{
						L("Adding lua (" << *lua_ << ")");
						board.add(Tag("c++", "include_path"), *lua_, tv);
						vector<file::File> files;
						MSS(file::read(files, *lua_));
						for (auto f: files)
						{
							if (f.extension() != "c")
								continue;
							if (f.basename() == "luac.c")
								continue;
							if (f.basename() == "lua.c")
								continue;
							board.add(Tag("c", "source"), f, tv);
						}
					}
				}

				if (sfml_ and roots_.count(*sfml_) == 0)
				{
					static const regex sfml_re("SFML/.+\\.hpp");
					if (regex_match(tv.second.string(), sfml_re))
					{
						L("Adding sfml (" << *sfml_ << ")");
						forest_.add(*sfml_, {"hpp", "cpp"});
						roots_.insert(*sfml_);
						{
							auto ip = *sfml_; ip << "include";
							board.add(Tag("c++", "include_path"), ip, tv);
						}
						{
							auto lp = *sfml_; lp << "build/lib";
							board.add(Tag("c++", "library_path"), lp, tv);
							board.add(Tag("c++", "library"), Value("sfml-system"), tv);
							board.add(Tag("c++", "library"), Value("sfml-graphics"), tv);
							board.add(Tag("c++", "library"), Value("sfml-window"), tv);
							board.add(Tag("c++", "library"), Value("sfml-audio"), tv);
							board.add(Tag("c++", "library"), Value("sfml-network"), tv);
						}
					}
				}

				L("Checking forest for " << tv.second.string());
				file::File rf;
				if (false) {}
				else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.second.string()), 1)))
					board.add(Tag("c++", "header"), rf, tv);
				else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.second.string()), 0)))
					board.add(Tag("c++", "header"), rf, tv);
				else
					L("Not found ...");
			}
			else if (tv.first == Tag("c++", "header"))
			{
				const auto header = tv.second.file();
				file::File source = header;
				{
					source.setExtension("cpp");
					if (forest_.contains(source))
						board.add(Tag("c++", "source"), source, tv);
				}
				{
					source.setExtension("c");
					if (forest_.contains(source))
						board.add(Tag("c", "source"), source, tv);
				}
			}
		}

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
