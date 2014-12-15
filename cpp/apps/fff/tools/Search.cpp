#include "fff/tools/Search.hpp"
#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
#include <regex>
#include <vector>
#include <set>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Search"
#include "gubg/log/begin.hpp"

namespace  { 
    typedef vector<string> Needles;
    typedef set<string> Extensions;
	enum class ReturnCode
	{
		MSS_DEFAULT_CODES,
        Skip, Stop,
	};
    class Finder
    {
        private:
            struct Hit
            {
                size_t line_nr;
                std::string txt;
            };

        public:
            Finder(const Needles &needles, const Extensions &extensions): needles_(needles), extensions_(extensions) {}

            ReturnCode recursor_discoveredFile(const file::File &f)
            {
                MSS_BEGIN(ReturnCode);
                if (searchFile_(f))
                {
                    L("Seaching " << f.name());
                    vector<Hit> hits;
                    string content;
                    MSS(file::read(content, f));
                    const auto lines = line::split(content);
                    for (auto it = lines.begin(); it != lines.end(); ++it)
                    {
                        if (lineContainsNeedles_(it->txt))
                            hits.push_back(Hit{size_t(it-lines.begin())+1, it->txt});
                    }
                    if (!hits.empty())
                    {
                        std::cout << std::endl << f.name() << std::endl;
                        for (const auto &hit: hits)
                            std::cout << hit.line_nr << "\t" << hit.txt << std::endl;
                    }
                }
                MSS_END();
            }

        private:
            bool searchFile_(const file::File &f) const
            {
                if (!file::isRegular(f))
                    return false;
                if (extensions_.empty())
                    return true;
                return extensions_.count(f.extension()) > 0;
            }
            bool lineContainsNeedles_(const string &txt) const
            {
                for (const auto &n: needles_)
                    if (string::npos == txt.find(n))
                        //Needle was not found
                        return false;
                //All needles were found, this is a hit
                return true;
            }
            const Needles needles_;
            const Extensions extensions_;
    };
} 

namespace fff { namespace tools { 

    ReturnCode Search::process(Board &board)
    {
        MSS_BEGIN(ReturnCode);

        auto tvs = board.getFrom(ix_);
        MSS(!tvs.empty(), NoNewEntries);
        ix_ += tvs.size();

        Extensions extensions;
        auto it = tvs.begin();
        for (; it != tvs.end(); ++it)
        {
            auto &tv = *it;
            if (false) {}
            else if (tv.tag == Tag("start"))
            {
                if (tv.value.string() == "search")
                {
                    //The search command indicates no more search options will come, all other
                    //start:* TVs will be interpreted as search keys
                    ++it;
                    break;
                }
            }
        }

        Needles needles;
        for (; it != tvs.end(); ++it)
        {
            auto &tv = *it;
            if (false) {}
            else if (tv.tag == Tag("start"))
            {
                L("Looking for searchkey \"" << tv.value.string() << "\"");
                needles.push_back(tv.value.string());
            }
            else if (tv.tag == Tag("search", "ext"))
            {
                L("Looking for extension \"" << tv.value.string() << "\"");
                extensions.insert(tv.value.string());
            }
        }

        Finder finder(needles, extensions);
        MSS(file::recurse(finder));

        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
