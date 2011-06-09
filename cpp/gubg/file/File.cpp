#include "file/File.hpp"
#define GUBG_LOG
#define LOG_LEVEL Warning
#include "logging/Log.hpp"
#include "boost/filesystem.hpp"
#include <fstream>
using namespace std;

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

        //Directory methods
        Directory::Ptr Directory::create(boost::filesystem::path path)
        {
            LOG_SM(Directory::create, "path: " << path.string());
            Ptr dir, ret;
            while (!path.empty())
            {
                LOG_M("path: " << path.string() << ", path.filename(): " << path.filename().string());

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
                    if (path.is_absolute())
                        dir->location_ = dir;
                }

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
            return boost::filesystem::exists(toPath());
        }

        bool Directory::isRoot() const
        {
            return this == location_.get(); 
        }
        string Directory::path() const {return toPath().string();}
        boost::filesystem::path Directory::toPath() const
        {
            if (!location_)
                return boost::filesystem::path(name_);
            return (isRoot() ? boost::filesystem::path("/") : location_->toPath() /= name_);
        }

        size_t Directory::expand(Ptr selfPtr, ExpandStrategy strategy)
        {
            size_t nrExpanded = 0;
            if (!selfPtr)
                return nrExpanded;
            Directory &self = *selfPtr;
            LOG_SM_(Debug, Directory::expand, "Expanding directory " << self.path() << " using strategy " << (int)strategy);
            switch (strategy)
            {
                case ExpandStrategy::Shallow:
                    {
                        self.childs_.clear();
                        auto path = self.toPath();
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
                                    LOG_M_(Warning, "Unknown file type " << p.string());
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
            LOG_SM(Regular::create, "filename: " << filename);
            Ptr ret;
            if (filename.empty())
                return ret;
            boost::filesystem::path path(filename);
            ret.reset(new Regular);
            LOG_M("Setting name_");
            ret->name_ = path.filename().string();
            path.remove_filename();
            LOG_M("Setting location_");
            ret->location_ = Directory::create(path);
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
            return boost::filesystem::exists(toPath());
        }

        string Regular::filepath() const
        {
            if (!location_)
                return name_;
            auto path = location_->toPath();
            path /= name();
            return path.string();
        }
        string Regular::extension() const
        {
            boost::filesystem::path path(name_);
            return path.extension().string();
        }
        boost::filesystem::path Regular::toPath() const
        {
            if (!location_)
                return boost::filesystem::path(name_);
            return location_->toPath() /= name_;
        }

        bool Regular::load(string &content)
        {
            LOG_SM(Regular::load, "Loading " << filepath());

            //Check that the file exists
            if (!exists())
            {
                LOG_M("File does not exist");
                return false;
            }

            //Open the file
            ifstream fi(filepath(), ifstream::binary);
            if (!fi.is_open())
            {
                LOG_M("Could not open the file");
                return false;
            }

            //Get the file size
            auto begin = fi.tellg();
            fi.seekg(0, ios::end);
            auto end = fi.tellg();
            auto size = end - begin;
            LOG_M("File size: " << size);

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
