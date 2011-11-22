#include "gubg/file/raw_file.hpp"
#include <sstream>
using namespace gubg::file::raw;
using namespace std;

//Directory
string Directory::path() const
{
    ostringstream oss;
    Nodes nodes = pathFromRoot();
    for (auto node = nodes.begin(); node != nodes.end(); ++node)
    {
        if (node != nodes.begin())
            oss << "/";
        oss << (*node)->name;
    }
    return oss.str();
}
ReturnCode expand(ExpandStrategy strategy)
{
    MSS_BEGIN(ReturnCode);
    MSS_END();
}

//Regular
string Regular::filename() const
{
    ostringstream oss;
    Nodes nodes = pathFromRoot();
    for (auto node = nodes.begin(); node != nodes.end(); ++node)
    {
        if (node != nodes.begin())
            oss << "/";
        oss << (*node)->name;
    }
    return oss.str();
}
