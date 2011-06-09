#include "model/FileSystem.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
#include "boost/system/system_error.hpp"
using namespace vix::model;
using namespace std;

FileSystem::FileSystem():
    root_(gubg::file::Directory::create("/"))
{
}

Path FileSystem::getPath(const string &path)
{
    LOG_SM_(Debug, FileSystem::getPath, "path " << path);
    auto dir = gubg::file::Directory::create(path);
    return getPath_(dir);
}

bool FileSystem::getFiles(Files &files, Path path)
{
    LOG_SM_(Debug, FileSystem::getFiles, "path: " << path);
    try
    {
        gubg::file::Directory::expand(path, gubg::file::ExpandStrategy::Shallow);
    }
    catch (boost::system::system_error &exc)
    {
        LOG_M_(Warning, "I cannot get the files: " << exc.what());
        return false;
    }
    files = path->childs();
    return true;
}

//Private methods
//Guarantee: getPath_ _always_ returns a Path inside our own filesystem tree, or an empty Path
Path FileSystem::getPath_(Path dir)
{
    LOG_SM_(Debug, FileSystem::getPath_, "dir: " << dir);
    //We basically go down to the root recursively and change all the locations and childs we find on our way down
    if (dir->isRoot())
    {
        //Roots always match, we return back our own root_ to ensure we are inside our own filesystem tree
        LOG_M_(Debug, "Return root_ " << root_);
        return root_;
    }

    //parent is guaranteed to be in our own filesystem tree, or an empty Path
    auto parent = getPath_(dir->location());
    if (!parent)
    {
        LOG_M_(Warning, "The parent doesn't even exist");
        return Path();
    }

    //Expand if it has no childs
    if (parent->empty())
        gubg::file::Directory::expand(parent, gubg::file::ExpandStrategy::Shallow);

    //If dir actually exists, dir->name() should match a child of parent; we will return that matching child, or and empty Path
    auto childs = parent->childs();
    for (auto it = childs.begin(); it != childs.end(); ++it)
    {
        auto &child = *it;
        if (child->name() == dir->name())
        {
            //Convert child (which is a gubg::file::File) into a gubg::file::Directory::Ptr (which is Path)
            auto ret = gubg::file::Directory::create(child);
            if (ret)
            {
                LOG_M_(Debug, "I found a matching child, and it is a directory too.");
            }
            else
            {
                LOG_M_(Warning, "I found a matching child, but it is not a directory.");
            }
            return ret;
        }
    }

    LOG_M_(Warning, "Could not find a matching child");
    return Path();
}
