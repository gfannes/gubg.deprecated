#include "fff/agents/ResolveHeader.hpp"
#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Variables.hpp"
#include <regex>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "ResolveHeader"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 
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
    bool gubg_inc_(file::File &f)
    {
        return gubg_env_var_(f, "GUBG_INC");
    }
    bool gubg_lib_(file::File &f)
    {
        return gubg_env_var_(f, "GUBG_LIB");
    }
    ResolveHeader::ResolveHeader()
    {
        file::File f;
        if (gubg_home_(f))
            gubg_.reset(new file::File(f << "cpp/libs/gubg"));
        if (gubg_home_(f))
            imui_.reset(new file::File(f << "cpp/libs/imui"));
        if (gubg_home_(f))
            lua_.reset(new file::File(f << "c/lua-5.2.3"));
        if (gubg_home_(f))
            catch_.reset(new file::File(f << "cpp/libs/catch"));
        if (gubg_home_(f))
            chai_.reset(new file::File(f << "cpp/libs/extern/ChaiScript/include"));
        if (gubg_home_(f))
            eigen_.reset(new file::File(f << "cpp/libs/extern/eigen"));
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
            else if (tv.tag == Tag("start"))
            {
                if (false) {}
                else
                {
                    file::File path(tv.value.as_string());
                    if (MSS_IS_OK(file::resolve(path)) and path.popBasename())
                    {
                        L("Adding local path to the forest (" << path << ")");
                        forest_.add(path, {"hpp", "cpp", "h", "c"});
                        {
                            auto ip = path; ip.popBasename();
                            board.addItem(Tag("c++", "include_path"), ip, tv);
                        }
                    }
                }
            }
            else if (tv.tag == Tag("c++", "tree"))
            {
                file::File path = tv.value.as_string();
                MSS(file::resolve(path));
                if (roots_.count(path) == 0)
                {
                    forest_.add(path, {"hpp", "cpp"});
                    roots_.insert(path);
                    auto ip = path; ip.popBasename();
                    board.addItem(Tag("c++", "include_path"), ip, tv);
                }
            }
            else if (tv.tag == Tag("c++", "utree"))
            {
                file::File path = tv.value.as_string();
                MSS(file::resolve(path));
                if (roots_.count(path) == 0)
                {
                    forest_.add(path, {"hpp", "cpp"});
                    roots_.insert(path);
                    auto ip = path;
                    board.addItem(Tag("c++", "include_path"), ip, tv);
                }
            }
            else if (tv.tag == Tag("c++", "include"))
            {
                if (gubg_ and roots_.count(*gubg_) == 0)
                {
                    static const regex gubg_re("gubg/.+\\.hpp");
                    if (regex_match(tv.value.as_string(), gubg_re))
                    {
                        L("Adding gubg (" << *gubg_ << ")");
                        forest_.add(*gubg_, {"hpp", "cpp"});
                        roots_.insert(*gubg_);
                        {
                            auto ip = *gubg_; ip.popBasename();
                            board.addItem(Tag("c++", "include_path"), ip, tv);
                        }
                    }
                }

                if (imui_ and roots_.count(*imui_) == 0)
                {
                    static const regex gubg_re("imui/.+\\.hpp");
                    if (regex_match(tv.value.as_string(), gubg_re))
                    {
                        L("Adding imui (" << *imui_ << ")");
                        forest_.add(*imui_, {"hpp", "cpp"});
                        roots_.insert(*imui_);
                        {
                            auto ip = *imui_; ip.popBasename();
                            board.addItem(Tag("c++", "include_path"), ip, tv);
                        }
                    }
                }

                if (catch_ and roots_.count(*catch_) == 0)
                {
                    if (tv.value.as_string() == "catch/catch.hpp")
                    {
                        L("Adding catch (" << *catch_ << ")");
                        forest_.add(*catch_, {"hpp", "cpp"});
                        roots_.insert(*catch_);
                        {
                            auto ip = *catch_; ip.popBasename();
                            board.addItem(Tag("c++", "include_path"), ip, tv);
                        }
                    }
                }

                if (eigen_ and roots_.count(*eigen_) == 0)
                {
                    if (tv.value.as_string() == "Eigen/Eigen")
                    {
                        L("Adding eigen (" << *eigen_ << ")");
                        roots_.insert(*eigen_);
                        board.addItem(Tag("c++", "include_path"), *eigen_, tv);
                    }
                }

                if (lua_ and roots_.count(*lua_) == 0)
                {
                    static const regex lua_re("lua.hpp");
                    if (regex_match(tv.value.as_string(), lua_re))
                    {
                        L("Adding lua (" << *lua_ << ")");
                        board.addItem(Tag("c++", "include_path"), *lua_, tv);
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
                            const Tag tag("c", "source");
                            board.setTypeForTag(tag, Type::File);
                            board.addItem(tag, f, tv);
                        }
                    }
                }

                if (sfml_ and roots_.count(*sfml_) == 0)
                {
                    static const regex sfml_re("SFML/.+\\.hpp");
                    if (regex_match(tv.value.as_string(), sfml_re))
                    {
                        L("Adding sfml (" << *sfml_ << ")");
                        forest_.add(*sfml_, {"hpp", "cpp"});
                        roots_.insert(*sfml_);
                        {
                            file::File ip;
                            if (gubg_inc_(ip))
                                board.addItem(Tag("c++", "include_path"), ip, tv);
                        }
                        {
                            file::File lp;
                            if (gubg_lib_(lp))
                                board.addItem(Tag("c++", "library_path"), lp, tv);
                            board.addItem(Tag("c++", "library"), Value("sfml-system"), tv);
                            board.addItem(Tag("c++", "library"), Value("sfml-graphics"), tv);
                            board.addItem(Tag("c++", "library"), Value("sfml-window"), tv);
                            board.addItem(Tag("c++", "library"), Value("sfml-audio"), tv);
                            board.addItem(Tag("c++", "library"), Value("sfml-network"), tv);
                            //board.addItem(Tag("c++", "library"), Value("GLEW"), tv);
                        }
                    }
                }

                if (chai_ and roots_.count(*chai_) == 0)
                {
                    static const regex chai_re("chaiscript/.+\\.hpp");
                    if (regex_match(tv.value.as_string(), chai_re))
                    {
                        L("Adding chai (" << *chai_ << ")");
                        forest_.add(*chai_, {"hpp", "cpp"});
                        roots_.insert(*chai_);
                        {
                            board.addItem(Tag("c++", "include_path"), *chai_, tv);
#ifdef GUBG_API_LINUX
                            board.addItem(Tag("c++", "library"), string("dl"), tv);
#endif
#if 0
                            for (auto str: vector<string>{"dispatchkit", "language", "utility"})
                            {
                                auto ip = *chai_; ip << str;
                                board.addItem(Tag("c++", "include_path"), ip, tv);
                            }
#endif
                        }
                    }
                }

                L("Checking forest for " << tv.value.as_string());
                file::File rf;
                if (false) {}
                else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.value.as_string()), 1)))
                {
                    const Tag tag("c++", "header");
                    board.setTypeForTag(tag, Type::File);
                    board.addItem(tag, rf, tv);
                }
                else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.value.as_string()), 0)))
                {
                    const Tag tag("c++", "header");
                    board.setTypeForTag(tag, Type::File);
                    board.addItem(tag, rf, tv);
                }
                else
                    L("Not found ...");
            }
            else if (tv.tag == Tag("c++", "header"))
            {
                const auto header = tv.value.as_file();
                file::File source = header;
                {
                    source.setExtension("cpp");
                    if (forest_.contains(source))
                    {
                        const Tag tag("c++", "source");
                        board.setTypeForTag(tag, Type::File);
                        board.addItem(tag, source, tv);

                    }
                }
                {
                    source.setExtension("c");
                    if (forest_.contains(source))
                    {
                        const Tag tag("c", "source");
                        board.setTypeForTag(tag, Type::File);
                        board.addItem(tag, source, tv);
                    }
                }
            }
        }

        MSS_END();
    }
} } 
#include "gubg/log/end.hpp"
