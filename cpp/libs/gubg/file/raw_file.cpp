#include "gubg/file/raw_file.hpp"
#include "gubg/file/basic.hpp"
#include "gubg/range.hpp"
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
    switch (expandStrategy)
    {
        case Shallow:
        case Recursive:
            {
                gubg::file::basic::Directories dirs;
                gubg::file::basic::Files files;
                MSS(gubg::file::basic::getDirectoryContent(dirs, files, path()), CouldNotGetDirContent);
                for (auto d = dirs.begin(); d != dirs.end(); ++d)
                {
                    auto &name = *d;
                    MSS(!name.empty(), EmptyEntry);
                    if (NoHiddenFiles == hiddenStrategy && name[0] == '.')
                        continue;
                    auto dir = new Directory;
                    dir->name = name;
                    MSS(add(dir), CouldNotAddEntry);
                }
                for (auto f = files.begin(); f != files.end(); ++f)
                {
                    auto &name = *f;
                    MSS(!name.empty(), EmptyEntry);
                    if (NoHiddenFiles == hiddenStrategy && name[0] == '.')
                        continue;
                    auto reg = new Regular;
                    reg->name = name;
                    MSS(add(reg), CouldNotAddEntry);
                }
                if (Recursive == expandStrategy)
                    for (auto r = rangeOnly<Directory*>(childs_); !r.empty(); r.popFront())
                        MSS(r->expand(expandStrategy, hiddenStrategy));
                break;
            }
        default: MSS_L(UnknownStrategy); break;
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