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

    ReturnCode ResolveHeader::process(Board &board)
    {
        MSS_BEGIN(ReturnCode);

        auto tvs = board.getFrom(ix_);
        MSS(!tvs.empty(), NoNewEntries);
        ix_ += tvs.size();

        //We first add all the tree _before_ we try to resolve the headers from the includes
        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == Tag("c++.tree"))
            {
                file::File path = tv.value.as_string();
                MSS(file::resolve(path));
                if (roots_.count(path) == 0)
                {
                    forest_.add(path, {"hpp", "cpp", "h", "c"});
                    roots_.insert(path);
                    auto ip = path; ip.popBasename();
                    board.addItem(Tag("c++.include_path"), ip, tv);
                }
            }
            else if (tv.tag == Tag("c++.utree"))
            {
                file::File path = tv.value.as_string();
                MSS(file::resolve(path));
                if (roots_.count(path) == 0)
                {
                    forest_.add(path, {"hpp", "cpp"});
                    roots_.insert(path);
                    auto ip = path;
                    board.addItem(Tag("c++.include_path"), ip, tv);
                }
            }
        }

        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == Tag("c++.include"))
            {
                L("Checking forest for " << tv.value.as_string());
                file::File rf;
                if (false) {}
                else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.value.as_string()), 1)))
                {
                    const Tag tag("c++.header");
                    board.addItem(tag, rf, tv);
                }
                else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.value.as_string()), 0)))
                {
                    const Tag tag("c++.header");
                    board.addItem(tag, rf, tv);
                }
                else
                    L("Not found ...");
            }
            else if (tv.tag == Tag("c++.header"))
            {
                const auto header = tv.value.as_file();
                file::File source = header;
                {
                    source.setExtension("cpp");
                    if (forest_.contains(source))
                    {
                        const Tag tag("c++.source");
                        board.addItem(tag, source, tv);

                    }
                }
                {
                    source.setExtension("c");
                    if (forest_.contains(source))
                    {
                        const Tag tag("c.source");
                        board.addItem(tag, source, tv);
                    }
                }
            }
        }

        MSS_END();
    }
} } 
#include "gubg/log/end.hpp"
