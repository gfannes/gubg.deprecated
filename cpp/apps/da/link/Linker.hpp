#ifndef HEADER_da_link_Linker_hpp_ALREADY_INCLUDED
#define HEADER_da_link_Linker_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "gubg/file/File.hpp"
#include <string>
#include <list>
#include <mutex>

namespace da
{
    namespace compile
    {
        class Linker
        {
            public:
                typedef std::string Setting;
                void addSetting(const Setting &);

                typedef gubg::file::File LibraryPath;
                void addLibraryPath(const LibraryPath &);

                typedef gubg::file::File ExeFile;
                typedef gubg::file::File ObjectFile;
                typedef std::list<ObjectFile> ObjectFiles;
                ReturnCode operator()(const ExeFile &, const ObjectFiles &);

            private:
                typedef std::list<Setting> Settings;
                Settings settings_;
                typedef std::list<gubg::file::File> LibraryPaths;
                LibraryPaths libraryPaths_;

                std::mutex mutex_;
        };
    }
}

#endif
