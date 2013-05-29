#include "pa/ShowWBS.hpp"
#include "pa/Model.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include <fstream>
using namespace pa;
using namespace std;

#define GUBG_MODULE_ "ShowWBS"
#include "gubg/log/begin.hpp"
namespace 
{
    struct Show
    {
        ostream &os;
        Show(ostream &o):os(o){}
        template <typename Path>
            bool open(Node &n, Path &p) const
            {
                const auto total = n.value*n.fraction;
                if (total <= 0)
                    return false;
                os << string(p.size()*2, ' ') << n.desc << " (" << total << ": " << n.fraction << " * " << n.value << ")" << endl;
                return true;
            }
        template <typename Path>
            void close(Node &n, Path &p) const
            {
            }
    };
}
pa::ReturnCode ShowWBS::execute(const Options &options)
{
	MSS_BEGIN(ReturnCode, "Showing WBS");

	gubg::tree::dfs::iterate(model(), Show(cout));

	if (!options.output.name().empty())
	{
		ofstream fo(options.output.name());
		gubg::tree::dfs::iterate(model(), Show(fo));
	}

	MSS_END();
}
