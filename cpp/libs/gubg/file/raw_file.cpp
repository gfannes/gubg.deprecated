#include "gubg/file/raw_file.hpp"
#include "gubg/file/basic.hpp"
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
ReturnCode Directory::expand(ExpandStrategy expandStrategy, HiddenStrategy hiddenStrategy)
{
    MSS_BEGIN(ReturnCode);
    if (Shallow == expandStrategy)
    {
        gubg::file::basic::Directories dirs;
        gubg::file::basic::Files files;
        MSS_T(gubg::file::basic::getDirectoryContent(dirs, files, path()), CouldNotGetDirContent);
        for (auto d = dirs.begin(); d != dirs.end(); ++d)
        {
            auto &name = *d;
            MSS_T(!name.empty(), EmptyEntry);
            if (NoHiddenFiles == hiddenStrategy && name[0] == '.')
                continue;
            auto dir = new Directory;
            dir->name = name;
            MSS_T(add(dir), CouldNotAddEntry);
        }
        for (auto f = files.begin(); f != files.end(); ++f)
        {
            auto &name = *f;
            MSS_T(!name.empty(), EmptyEntry);
            if (NoHiddenFiles == hiddenStrategy && name[0] == '.')
                continue;
            auto reg = new Regular;
            reg->name = name;
            MSS_T(add(reg), CouldNotAddEntry);
        }
    }
    else
    {
        MSS_L(UnknownStrategy);
    }
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
