#include "fff/agents/ResolveHeader.hpp"
#include "fff/Tags.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Variables.hpp"
#include <regex>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "ResolveHeader"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

    gubg::bbs::ReturnCode ResolveHeader::process(gubg::bbs::Board &board)
    {
        MSS_BEGIN(gubg::bbs::ReturnCode);

        auto tvs = board.getFrom(ix_);
        MSS(!tvs.empty(), NoNewEntries);
        ix_ += tvs.size();

        //We first add all the tree _before_ we try to resolve the headers from the includes
        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == cpp_tree)
            {
                file::File path = tv.value;
                MSS(file::resolve(path));
                if (roots_.count(path) == 0)
                {
                    forest_.add(path, {"hpp", "cpp", "h", "c"});
                    roots_.insert(path);
                    auto ip = path; ip.popBasename();
                    board.addItem(cpp_include_path, ip, tv);
                }
            }
            else if (tv.tag == cpp_utree)
            {
                file::File path = tv.value;
                MSS(file::resolve(path));
                if (roots_.count(path) == 0)
                {
                    forest_.add(path, {"hpp", "cpp"});
                    roots_.insert(path);
                    auto ip = path;
                    board.addItem(cpp_include_path, ip, tv);
                }
            }
        }

        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == cpp_include)
            {
                L("Checking forest for " << tv.value);
                file::File rf;
                if (false) {}
                else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.value), 1)))
                {
                    board.addItem(cpp_header, rf, tv);
                }
                else if (MSS_IS_OK(forest_.resolve(rf, file::File(tv.value), 0)))
                {
                    board.addItem(cpp_header, rf, tv);
                }
                else
                    L("Not found ...");
            }
            else if (tv.tag == cpp_header)
            {
                const file::File header{tv.value};
                file::File source = header;
                {
                    source.setExtension("cpp");
                    if (forest_.contains(source))
                        board.addItem(cpp_source, source, tv);
                }
                {
                    source.setExtension("c");
                    if (forest_.contains(source))
                        board.addItem(c_source, source, tv);
                }
            }
        }

        MSS_END();
    }
} } 
#include "gubg/log/end.hpp"
