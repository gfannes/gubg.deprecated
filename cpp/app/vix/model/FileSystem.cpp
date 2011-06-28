#include "model/FileSystem.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
#include "boost/system/system_error.hpp"
using namespace vix::model;
using namespace std;

ostream &operator<<(ostream &os, Path path)
{
    Path::Unlock unlockedPath(path);
    return os << unlockedPath.ptr();
}

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
    Path::Unlock unlockedPath(path);
    try
    {
        gubg::file::Directory::expand(unlockedPath.ptr(), gubg::file::ExpandStrategy::Shallow);
    }
    catch (boost::system::system_error &exc)
    {
        LOG_M_(Warning, "I cannot get the files: " << exc.what());
        return false;
    }
    files.clear();
    auto childs = unlockedPath->childs();
    for (auto ch = childs.begin(); ch != childs.end(); ++ch)
        files.push_back(File(*ch));
    return true;
}

Path FileSystem::toPath(File &file)
{
    File::Unlock unlocked(file);
    auto path = boost::dynamic_pointer_cast<Path::Data>(unlocked.ptr());
    if (path)
        return Path(path);
    return Path();
}

Regular FileSystem::toRegular(File &file)
{
    File::Unlock unlocked(file);
    auto regular = boost::dynamic_pointer_cast<Regular::Data>(unlocked.ptr());
    if (regular)
        return Regular(regular);
    return Regular();
}

//Private methods
//Guarantee: getPath_ _always_ returns a Path inside our own filesystem tree, or an empty Path
Path FileSystem::getPath_(Path dir)
{
    LOG_SM_(Debug, FileSystem::getPath_, "dir: " << dir);
    Path::Unlock unlockedDir(dir);
    //We basically go down to the root recursively and change all the locations and childs we find on our way down
    if (unlockedDir->isRoot())
    {
        //Roots always match, we return back our own root_ to ensure we are inside our own filesystem tree
        LOG_M_(Debug, "Return root_ " << root_);
        return root_;
    }

    //parent is guaranteed to be in our own filesystem tree, or an empty Path
    auto parent = getPath_(unlockedDir->location());
    Path::Unlock unlockedParent(parent);
    if (!unlockedParent())
    {
        LOG_M_(Warning, "The parent doesn't even exist");
        return Path();
    }

    //Expand if it has no childs
    if (unlockedParent->empty())
        gubg::file::Directory::expand(unlockedParent.ptr(), gubg::file::ExpandStrategy::Shallow);

    //If dir actually exists, dir->name() should match a child of parent; we will return that matching child, or and empty Path
    auto childs = unlockedParent->childs();
    for (auto it = childs.begin(); it != childs.end(); ++it)
    {
        auto &child = *it;
        if (child->name() == unlockedDir->name())
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
