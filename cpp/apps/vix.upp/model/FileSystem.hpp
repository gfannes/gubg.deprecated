#ifndef vix_model_FileSystem_hpp
#define vix_model_FileSystem_hpp

#include "gubg/Singleton.hpp"
#include "gubg/pointer/Locked.hpp"
#include "gubg/file/File.hpp"
#include "boost/signals2.hpp"
#include <string>

namespace vix
{
    namespace model
    {
        typedef gubg::pointer::Locked<gubg::file::Directory, gubg::pointer::ThreadSafeBaseType, gubg::file::File> Path;
        typedef gubg::pointer::Locked<gubg::file::Regular, gubg::pointer::ThreadSafeBaseType, gubg::file::File> Regular;
        typedef gubg::pointer::Locked<gubg::file::File, gubg::pointer::ThreadSafeBaseType, gubg::file::File> File;
#if 0
        //TODO::These are not thread-safe, use pointer::Locked instead, making sure there is a single mutex both for Directory and Regular
        typedef gubg::file::Directory::Ptr Path;
        typedef gubg::file::File::Ptr File;
#endif
        typedef std::vector<File> Files;

        enum class DeleteStrategy {Single, Recursive};

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
                bool getFiles(Files &, Path, bool recursive);

                Path toPath(File &);
                Regular toRegular(File &);

                typedef boost::signals2::signal<void ()> UpdateSignal;
                boost::signals2::connection connect(const UpdateSignal::slot_type &subscriber);

                bool createSubDirectory(Path, const std::string &dirname);
                bool createSubRegular(Path, const std::string &filename);
                bool remove(File, DeleteStrategy);

            private:
                Path getPath_(Path);

                Path root_;

                UpdateSignal updated_;
        };
    }
}

#endif
