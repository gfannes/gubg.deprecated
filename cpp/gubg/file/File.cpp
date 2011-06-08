#include "file/File.hpp"
#define GUBG_LOG
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

        bool Directory::exists() const
        {
            return boost::filesystem::exists(toPath());
        }

        bool Directory::isRoot() const
        {
            return this == location_.get(); 
        }
        boost::filesystem::path Directory::toPath() const
        {
            if (!location_)
                return boost::filesystem::path(name_);
            return (isRoot() ? boost::filesystem::path("/") : location_->toPath() /= name_);
        }

        void Directory::expand(ExpandStrategy strategy)
        {
            switch (strategy)
            {
                case ExpandStrategy::Shallow:
                    childs_.clear();
                    auto path = toPath();
                    for (auto it = directory_iterator(path); it != directory_iterator(); ++it)
                    {
                        auto p = it->path();
                        File::Ptr file;
                        if (boost::filesystem::is_regular_file(p))
                        {
                            file.reset(new Regular);
                        }
                        else if (boost::filesystem::is_directory(p))
                        {
                            file.reset(new Directory);
                        }
                        else
                        {
                        }
                    }
                    break;
                case ExpandStrategy::Shallow:
                    throw string("Not implemented yet");
                    break;
            }
        }
    }
}
