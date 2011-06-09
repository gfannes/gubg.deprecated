#ifndef vix_model_FileSystem_hpp
#define vix_model_FileSystem_hpp

#include "Singleton.hpp"
#include "file/File.hpp"
#include <string>

namespace vix
{
    namespace model
    {
        typedef gubg::file::Directory::Ptr Path;
        typedef gubg::file::File::Ptr File;
        typedef std::vector<File> Files;

        //This singleton holds the filesystem tree
        //All filesystem operation should go via here, and not directly on the handed-out objects
        class FileSystem
        {
            private:
                friend class gubg::Singleton<FileSystem>;
                FileSystem();

            public:
                static FileSystem &instance() {return gubg::Singleton<FileSystem>::instance();}

                Path getPath(const std::string &path);
                //It is assumed that Path was handed-out by FileSystem _directly_
                //Returns false if the files could not be retrieved, e.g., due to permission problems
                bool getFiles(Files &, Path);

            private:
                Path getPath_(Path);

                Path root_;
        };
    }
}

#endif
