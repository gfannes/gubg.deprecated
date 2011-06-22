#ifndef gubg_file_File_hpp
#define gubg_file_File_hpp

#include "boost/shared_ptr.hpp"
#include "boost/filesystem/path.hpp"
#include <string>
#include <vector>
#include <ostream>

namespace gubg
{
    namespace file
    {
        class Directory;

        class File
        {
            public:
                typedef boost::shared_ptr<File> Ptr;

                bool isAbsolute() const;
                bool isRelative() const;
                bool isHidden() const;
                bool isDirectory() const;
                bool isRegular() const;

                virtual bool exists() const = 0;

                std::string name() const {return name_;}
                //Watch out, location() can return an empty ptr
                typedef boost::shared_ptr<Directory> DirectoryPtr;
                DirectoryPtr location() const {return location_;}
                void setLocation(DirectoryPtr location) {location_ = location;}

            protected:
                std::string name_;
                DirectoryPtr location_;
        };

        enum class ExpandStrategy
        {
            Shallow,
            Recursive,
        };
        class Directory: public File
        {
            private:
                Directory(){}

            public:
                typedef boost::shared_ptr<Directory> Ptr;
                //Creation from a std::string
                static Ptr create(boost::filesystem::path path);
                static Ptr create(const std::string &name, Directory::Ptr location);
                //Downcast from a File::Ptr
                static Ptr create(File::Ptr &);

                //File interface
                virtual bool exists() const;

                bool isRoot() const;
                std::string path() const;
                boost::filesystem::path toPath() const;
                static size_t expand(Ptr self, ExpandStrategy);
                bool empty() const {return childs_.empty();}
                typedef std::vector<File::Ptr> Childs;
                Childs childs() const {return childs_;}

            private:
                Childs childs_;
        };

        class Regular: public File
        {
            private:
                friend class Directory;
                Regular(){}

            public:
                typedef boost::shared_ptr<Regular> Ptr;
                static Ptr create(const std::string &filename);
                static Ptr create(const std::string &name, Directory::Ptr location);
                //Downcast from a File::Ptr
                static Ptr create(File::Ptr &);

                //File interface
                virtual bool exists() const;

                std::string filepath() const;
                std::string extension() const;
                boost::filesystem::path toPath() const;
                bool load(std::string &content);
        };
    }
}

std::ostream &operator<<(std::ostream &, const gubg::file::File::Ptr &);
std::ostream &operator<<(std::ostream &, const gubg::file::Directory::Ptr &);
std::ostream &operator<<(std::ostream &, const gubg::file::Regular::Ptr &);

#endif
