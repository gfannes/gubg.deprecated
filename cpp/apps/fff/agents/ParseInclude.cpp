#include "fff/agents/ParseInclude.hpp"
#include "fff/Tags.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Variables.hpp"
#include <regex>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "ParseInclude"
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
    ParseInclude::ParseInclude()
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
        if (gubg_home_(f))
            poco_.reset(new file::File(f << "cpp/libs/extern/poco"));
        if (gubg_sdks_(f))
            sfml_.reset(new file::File(f << "SFML"));
    }
    gubg::bbs::ReturnCode ParseInclude::process(gubg::bbs::Board &board)
    {
        MSS_BEGIN(gubg::bbs::ReturnCode);

        auto tvs = board.getFrom(ix_);
        MSS(!tvs.empty(), NoNewEntries);
        ix_ += tvs.size();

        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == start)
            {
                if (false) {}
                else
                {
                    file::File path(tv.value);
                    if (MSS_IS_OK(file::resolve(path)) && path.popBasename())
                    {
                        L("Adding local path to the forest (" << path << ")");
                        board.addItem(cpp_tree, path, tv);
                    }
                }
            }
            else if (tv.tag == cpp_include)
            {
                if (gubg_)
                {
                    static const regex gubg_re("gubg/.+\\.hpp");
                    if (regex_match(tv.value, gubg_re))
                    {
                        L("Adding gubg (" << *gubg_ << ")");
                        board.addItem(cpp_tree, *gubg_, tv);
                    }
                }

                if (imui_)
                {
                    static const regex gubg_re("imui/.+\\.hpp");
                    if (regex_match(tv.value, gubg_re))
                    {
                        L("Adding imui (" << *imui_ << ")");
                        board.addItem(cpp_tree, *imui_, tv);
                    }
                }

                if (catch_)
                {
                    if (tv.value == "catch/catch.hpp")
                    {
                        L("Adding catch (" << *catch_ << ")");
                        board.addItem(cpp_tree, *catch_, tv);
                    }
                }

                if (eigen_)
                {
                    if (tv.value == "Eigen/Eigen")
                    {
                        L("Adding eigen (" << *eigen_ << ")");
                        board.addItem(cpp_include_path, *eigen_, tv);
                    }
                }

                if (poco_)
                {
                    static const regex poco_re("Poco/.+\\.h");
                    if (regex_match(tv.value, poco_re))
                    {
                        L("Adding poco (" << *poco_ << ")");
                        {
                            file::File ip = *poco_; ip << "Foundation/include";
                            board.addItem(cpp_include_path, ip, tv);
                        }
                        {
                            file::File lp = *poco_; lp << "lib/Linux/x86_64";
                            board.addItem(cpp_library_path, lp, tv);
                            board.addItem(cpp_library, "PocoFoundation", tv);
                        }
                    }
                }

                if (lua_)
                {
                    static const regex lua_re("lua.hpp");
                    if (regex_match(tv.value, lua_re))
                    {
                        L("Adding lua (" << *lua_ << ")");
                        board.addItem(cpp_include_path, *lua_, tv);
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
                            board.addItem(c_source, f, tv);
                        }
                    }
                }

                if (sfml_)
                {
                    static const regex sfml_re("SFML/.+\\.hpp");
                    if (regex_match(tv.value, sfml_re))
                    {
                        L("Adding sfml (" << *sfml_ << ")");
                        board.addItem(cpp_tree, *sfml_, tv);
                        {
                            file::File ip;
                            if (gubg_inc_(ip))
                                board.addItem(cpp_include_path, ip, tv);
                        }
                        {
                            file::File lp;
                            if (gubg_lib_(lp))
                                board.addItem(cpp_library_path, lp, tv);
                            board.addItem(cpp_library, "sfml-system", tv);
                            board.addItem(cpp_library, "sfml-graphics", tv);
                            board.addItem(cpp_library, "sfml-window", tv);
                            board.addItem(cpp_library, "sfml-audio", tv);
                            board.addItem(cpp_library, "sfml-network", tv);
                            //board.addItem(cpp_library, "GLEW", tv);
                        }
                    }
                }

                if (chai_)
                {
                    static const regex chai_re("chaiscript/.+\\.hpp");
                    if (regex_match(tv.value, chai_re))
                    {
                        L("Adding chai (" << *chai_ << ")");
                        board.addItem(cpp_utree, *chai_, tv);
                        {
#ifdef GUBG_API_LINUX
                            board.addItem(cpp_library, string("dl"), tv);
#endif
#if 0
                            for (auto str: vector<string>{"dispatchkit", "language", "utility"})
                            {
                                auto ip = *chai_; ip << str;
                                board.addItem(cpp_include_path, ip, tv);
                            }
#endif
                        }
                    }
                }
            }
        }

        MSS_END();
    }
} } 
#include "gubg/log/end.hpp"
