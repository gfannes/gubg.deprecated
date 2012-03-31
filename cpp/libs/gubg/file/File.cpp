#include "gubg/file/File.hpp"
#define GUBG_MODULE "gubg::file"
#define GUBG_LOG
#define LOG_LEVEL Warning
#include "gubg/logging/Log.hpp"
#include "gubg/Platform.hpp"
#include "boost/filesystem.hpp"
#include <fstream>
#include "gubg/nullptr.hpp"
#ifdef GUBG_WIN32
#include "windows.h"
#endif
using namespace std;

namespace
{
    bool isAbsolute_(const boost::filesystem::path &path)
    {
#ifdef GUBG_WIN32
        string str = path.string();
        if (str.size() >= 2 && ':' == str[1])
            return true;
        return false;
#else
        return path.is_absolute();
#endif
    }
}

std::ostream &operator<<(std::ostream &os, const gubg::file::File::Ptr &file)
{
    LOG_S_(Debug, stream, "File");
    return os << file->name();
}
std::ostream &operator<<(std::ostream &os, const gubg::file::Directory::Ptr &dir)
{
    LOG_S_(Debug, stream, "Directory: " << dir.get());
    return os << dir->path();
}
std::ostream &operator<<(std::ostream &os, const gubg::file::Regular::Ptr &regular)
{
    LOG_S_(Debug, stream, "Regular");
    return os << regular->filepath();
}

namespace gubg
{
    namespace file
    {
        //File methods
        bool File::isAbsolute() const
        {
            Directory::Ptr dir = location_;
            while (dir)
            {
                if (dir == dir->location_)
                    return true;
                dir = dir->location_;
            }
            return false;
        }
        bool File::isRelative() const {return !isAbsolute();}
        bool File::isHidden() const
        {
            if (name_.empty())
                throw string("I did not expect an empty name_");
            return name_[0] == '.';
        }
        bool File::isDirectory() const
        {
            return nullptr != dynamic_cast<const Directory*>(this);
        }
        bool File::isRegular() const
        {
            return nullptr != dynamic_cast<const Regular*>(this);
        }

        //Directory methods
        Directory::Ptr Directory::create(const std::string &p)
        {
            string tmpPath = p;
            LOG_S_(Info, Directory::create, "path: " << tmpPath);

            if (tmpPath == "/")
            {
                //The root of the filesystem is requested
                Ptr root(new Directory);
#ifdef GUBG_WIN32
                root->name_ = "";
#else
                root->name_ = "/";
#endif
                root->location_ = root;
                return root;
            }

#ifdef GUBG_LINUX
            //If we don't remove any trailing '/', boost::filesystem::path will return "/asd/asdf/./"
            if (tmpPath[tmpPath.size()-1] == '/')
                tmpPath.resize(tmpPath.size()-1);
#endif

            boost::filesystem::path path(tmpPath);
            Ptr dir, ret;
            while (!path.empty())
            {
#ifdef GUBG_WIN32
                if (path.filename().string() == "\\" || path.filename().string() == "/")
                    goto SkipSpecialName;
#endif
                LOG_M_(Info, "path: " << path.string() << ", path.filename(): " << path.filename().string());

                {
                    Ptr tmp(new Directory);
                    tmp->name_ = path.filename().string();

                    if (!ret)
                        dir = ret = tmp;
                    else
                    {
                        dir->location_ = tmp;
                        dir = tmp;
                    }

                    if (!path.has_parent_path())
                    {
                        LOG_M_(Debug, "There is no parent anymore");
                        if (isAbsolute_(path))
                        {
                            LOG_M_(Debug, "This is an absolute path");
#ifdef GUBG_WIN32
                            Ptr root(new Directory);
                            LOG_M_(Debug, "Root is " << root.get());
                            root->name_ = "";
                            root->location_ = root;
                            dir->location_ = root;
#else
                            dir->location_ = dir;
#endif
                        }
                    }
                    else
                    {
                        LOG_M_(Debug, "There is a parent path");
                    }
                    LOG_M_(Debug, "dir is " << dir.get());
                }

#ifdef GUBG_WIN32
SkipSpecialName:
#endif
                path.remove_filename();
            }
            return ret;
        }
        Directory::Ptr Directory::create(const string &name, Directory::Ptr location)
        {
            Ptr dir(new Directory);
            dir->name_ = name;
            dir->location_ = location;
            return dir;
        }
        Directory::Ptr Directory::create(File::Ptr &file)
        {
            return boost::dynamic_pointer_cast<Directory>(file);
        }

        bool Directory::exists() const
        {
            return boost::filesystem::exists(path());
        }

        bool Directory::isRoot() const
        {
            LOG_S_(Debug, isRoot, this);
            //The root has itself as its location
            return this == location_.get(); 
        }
        string Directory::path() const
        {
            LOG_S_(Debug, path, "");
            if (!location_)
                return name_;
#ifdef GUBG_WIN32
            if (isRoot())
            {
                LOG_M_(Debug, "is root");
                return "";
            }
            else if (location_->isRoot())
            {
                LOG_M_(Debug, "parent is root");
                return name_;
            }
            else
            {
                LOG_M_(Debug, "something else");
                return location_->path() + "/" + name_;
            }
#else
            string str = (isRoot() ? "/" : location_->path() + name_ + "/");
            LOG_M_(Debug, "Directory::path: " << str);
            return str;
#endif
        }

        size_t Directory::expand(Ptr selfPtr, ExpandStrategy strategy)
        {
            size_t nrExpanded = 0;
            if (!selfPtr)
                return nrExpanded;
            Directory &self = *selfPtr;
            LOG_S_(Debug, Directory::expand, "Expanding directory " << self.path() << " using strategy " << (int)strategy);
            switch (strategy)
            {
                case ExpandStrategy::Shallow:
                    {
                        LOG_M_(Debug, "Shallow expansion");
                        self.childs_.clear();
#ifdef GUBG_WIN32
                        if (self.isRoot())
                        {
                            LOG_M_(Debug, "Getting the logical drives for windows");
                            //For windows, we cannot use boost to iterate over the drives
                            DWORD logicalDrives = ::GetLogicalDrives();
                            string drive("?:");
                            for (int i = 0; i < 26; ++i)
                            {
                                if (logicalDrives & (1 << i))
                                {
                                    drive[0] = 'a'+i;
                                    self.childs_.push_back(Directory::create(drive, selfPtr));
                                }
                            }
                            return self.childs_.size();
                        }
#endif
                        boost::filesystem::path path = self.path();
                        for (auto it = boost::filesystem::directory_iterator(path); it != boost::filesystem::directory_iterator(); ++it)
                        {
                            auto p = it->path();
                            File::Ptr file;
                            if (boost::filesystem::is_regular_file(p))
                            {
                                LOG_M_(Debug, "Creating regular file " << p.filename().string());
                                file = Regular::create(p.filename().string(), selfPtr);
                            }
                            else if (boost::filesystem::is_directory(p))
                            {
                                if (!boost::filesystem::is_symlink(p))
                                {
                                    LOG_M_(Debug, "Creating directory " << p.filename().string());
                                    file = Directory::create(p.filename().string(), selfPtr);
                                }
                            }
                            else
                            {
                                if (boost::filesystem::is_symlink(p))
                                {
                                    LOG_M_(Info, "Unexisting symlink found: " << p.string());
                                }
                                else
                                {
                                    LOG_M_(Error, "Unknown file type " << p.string());
                                }
                            }
                            if (file)
                                self.childs_.push_back(file);
                        }
                        nrExpanded = self.childs_.size();
                    }
                    break;
                case ExpandStrategy::Recursive:
                    {
                        LOG_M_(Debug, "Recursive expansion");
                        nrExpanded = expand(selfPtr, ExpandStrategy::Shallow);
                        for (auto child = self.childs_.begin(); child != self.childs_.end(); ++child)
                        {
                            auto dir = Directory::create(*child); 
                            if (dir)
                            {
                                LOG_M_(Debug, dir->path());
                                nrExpanded += expand(dir, ExpandStrategy::Recursive);
                            }
                        }
                    }
                    break;
            }
            LOG_M_(Debug, "I found " << nrExpanded << " files (regular+directory)");
            return nrExpanded;
        }

        //Regular methods
        Regular::Ptr Regular::create(const string &filename)
        {
            LOG_S(Regular::create, "filename: " << filename);
            Ptr ret;
            if (filename.empty())
                return ret;
            boost::filesystem::path path(filename);
            ret.reset(new Regular);
            LOG_M("Setting name_ to " << path.filename().string());
            ret->name_ = path.filename().string();
            path.remove_filename();
            LOG_M("Setting location_");
            ret->location_ = Directory::create(path.string());
            return ret;
        }
        Regular::Ptr Regular::create(const string &name, Directory::Ptr location)
        {
            Ptr regular(new Regular);
            regular->name_ = name;
            regular->location_ = location;
            return regular;
        }
        Regular::Ptr Regular::create(File::Ptr &file)
        {
            return boost::dynamic_pointer_cast<Regular>(file);
        }

        bool Regular::exists() const
        {
            return boost::filesystem::exists(boost::filesystem::path(filepath()));
        }

        string Regular::filepath() const
        {
            if (!location_)
                return name_;
#ifdef GUBG_WIN32
            auto path = location_->path() + "/" + name();
#else
            auto path = location_->path() + name();
#endif
            return path;
        }
        string Regular::extension() const
        {
            boost::filesystem::path path(name_);
            return path.extension().string();
        }

        bool Regular::load(string &content)
        {
            LOG_S_(Warning, Regular::load, "Loading " << filepath());

            //Check that the file exists
            if (!exists())
            {
                LOG_M_(Error, "File does not exist");
                return false;
            }

            //Open the file
            ifstream fi(filepath(), ifstream::binary);
            if (!fi.is_open())
            {
                LOG_M_(Error, "Could not open the file");
                return false;
            }

            //Get the file size
            auto begin = fi.tellg();
            fi.seekg(0, ios::end);
            auto end = fi.tellg();
            auto size = end - begin;
            LOG_M_(Info, "File size: " << size);

            //Allocate enough space to fit the file
            content.resize(size);

            //Read the file
            fi.seekg(0, ios::beg);
            fi.read(&content[0], size);
            fi.close();

            return true;
        }
    }
}
