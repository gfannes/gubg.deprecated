#include "fff/agents/FixIncludeGuard.hpp"
#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
#include <string>
#include <set>
#include <iostream>
#include <cassert>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "FixIncludeGuard"
#include "gubg/log/begin.hpp"
namespace {
    using Extensions = set<string>;
    class IGFixer
    {
        public:
            IGFixer(file::File dir, const Extensions &exts, bool fix_it): dir_(dir), exts_(exts), fix_it_(fix_it)
            {
                needles_.insert('/');
                needles_.insert('.');
                needles_.insert(' ');
            }

            enum class ReturnCode { OK, Skip, Stop };
            ReturnCode recursor_discoveredFile(file::File f)
            {
                MSS_BEGIN(ReturnCode);
                if (isRegular(f))
                {
                    MSS_Q(exts_.count(f.extension()) == 1, Skip);
                    auto rel = f.relative(dir_);
                    L(STREAM(f, rel, fix_it_));
                    string content;
                    MSS(read(content, f), Skip);
                    auto lines = line::split(content);
                    MSS_Q(lines.size() >= 3, Skip);
                    MSS_Q(lines[0].txt.substr(0, 8) == "#ifndef ", Skip);
                    MSS_Q(lines[1].txt.substr(0, 8) == "#define ", Skip);
                    MSS_Q(lines.back().txt.substr(0, 6) == "#endif", Skip);
                    const auto wanted_define = define_(rel.name());
                    MSS_Q(lines[0].txt.substr(8) != wanted_define || lines[1].txt.substr(8) != wanted_define, Skip);
                    cout << "Include guard problem detected for " << f << endl;
                    lines[0].txt = "#ifndef "; lines[0].txt += wanted_define;
                    lines[1].txt = "#define "; lines[1].txt += wanted_define;
                    content = line::join(lines);
                    if (fix_it_)
                    {
                        cout << "    => changing to " << wanted_define << endl;
                        MSS(write(content, f), Skip);
                    }
                }
                MSS_END();
            }
        private:
            string define_(string rel) const
            {
                for (auto &ch: rel)
                    if (needles_.count(ch) == 1)
                        ch = '_';
                return string{"HEADER_"}+rel+"_ALREADY_INCLUDED";
            }
            const file::File dir_;
            const Extensions &exts_;
            const bool fix_it_;
            set<char> needles_;
    };
}
namespace fff { namespace agents { 
	ReturnCode FixIncludeGuard::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

        board.addItem(Tag{"ig.ext"}, string{"hpp"});
        board.addItem(Tag{"ig.ext"}, string{"h"});
        if (board.isDirty())
            MSS_RETURN_OK();

		auto tvs = board.getFrom(ix_);
		MSS(!tvs.empty(), NoNewEntries);
		ix_ += tvs.size();

        Extensions extensions;
        bool fix_it = false;
		for (auto tv: tvs)
		{
			if (tv.tag == Tag("ig.ext"))
                extensions.insert(tv.value.as_string());
			if (tv.tag == Tag("ig.fix"))
            {
                if (tv.value == Value{"true"})
                    fix_it = true;
                if (tv.value == Value{"false"})
                    fix_it = false;
            }
		}

        const auto wd = file::getcwd();
        IGFixer fixer{wd, extensions, fix_it};
        file::recurse(fixer, wd);

		MSS_END();
	}
} } 
#include "gubg/log/end.hpp"
