#include "pa/ShowDebug.hpp"
#include "pa/Model.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include <fstream>
#include <algorithm>
using namespace pa;
using namespace std;

#define GUBG_MODULE_ "ShowDebug"
#include "gubg/log/begin.hpp"
namespace 
{
    struct Show
    {
		const Options &options;
        ostream &os;
		mutable Node *line;

        Show(const Options &opts, ostream &o):options(opts), os(o){}

        template <typename Path>
            bool open(Node &n, Path &p) const
			{
				if (p.empty())
				{
					line = 0;
				}
				else
				{
					if (line == 0)
					{
						if (!options.lines.empty() && count(options.lines.begin(), options.lines.end(), n.desc) == 0)
							return false;
						line = &n;
					}
					os << string(p.size()*2, ' ') << n.desc << " (cumul: " << n.cumul << ", fraction: " << n.fraction << ", total: " << n.total() << ", id: " << n.id << ")" << endl;
					for (auto attr: n.attributes)
						os << string(p.size()*2, ' ') << "[" << attr.first << ": " << attr.second << "]" << endl;
				}
				return true;
			}
        template <typename Path>
            void close(Node &n, Path &p) const
            {
				if (line == &n)
					line = 0;
            }
    };
}
pa::ReturnCode ShowDebug::execute(const Options &options)
{
	MSS_BEGIN(ReturnCode, "Showing Debug");

	gubg::tree::dfs::iterate(model(), Show(options, cout));

	if (!options.output.name().empty())
	{
		ofstream fo(options.output.name());
		gubg::tree::dfs::iterate(model(), Show(options, fo));
	}

	MSS_END();
}
