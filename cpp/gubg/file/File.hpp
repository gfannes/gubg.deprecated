#ifndef gubg_file_File_hpp
#define gubg_file_File_hpp

#include "boost/shared_ptr.hpp"
#include "boost/filesystem/path.hpp"
#include <string>
#include <vector>

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

                virtual bool exists() const = 0;

                std::string name() const {return name_;}

            protected:
                std::string name_;
                boost::shared_ptr<Directory> location_;
        };
        class Regular: public File
        {
            private:
                Regular(){}

            public:
                typedef boost::shared_ptr<Regular> Ptr;
                static Ptr create(const std::string &filename);

                //File interface
                virtual bool exists() const;

                std::string filepath() const;
                std::string extension() const;
                boost::filesystem::path toPath() const;
                bool load(std::string &content);
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
                static Ptr create(boost::filesystem::path path);

                //File interface
                virtual bool exists() const;

                bool isRoot() const;
                std::string path() const;
                boost::filesystem::path toPath() const;
                void expand(ExpandStrategy);

            private:
                typedef std::vector<File::Ptr> Childs;
                Childs childs_;
        };
    }
}

#endif
