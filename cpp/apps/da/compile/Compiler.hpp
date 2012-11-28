#ifndef HEADER_da_compile_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_da_compile_Compiler_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "gubg/file/File.hpp"
#include <string>
#include <mutex>

namespace da
{
    namespace compile
    {
        class Compiler
        {
            public:
                typedef std::string Define;
                void addDefine(const Define &);

                typedef std::string Setting;
                void addSetting(const Setting &);

                typedef gubg::file::File IncludePath;
                void addIncludePath(const IncludePath &);

                typedef gubg::file::File SourceFile;
                typedef gubg::file::File ObjectFile;
                ReturnCode operator()(const ObjectFile &, const SourceFile &);

                typedef std::list<ObjectFile> ObjectFiles;
                const ObjectFiles &objectFiles() const {return objectFiles_;}

            private:
                typedef std::list<Define> Defines;
                Defines defines_;
                typedef std::list<Setting> Settings;
                Settings settings_;
                typedef std::list<gubg::file::File> IncludePaths;
                IncludePaths includePaths_;
                ObjectFiles objectFiles_;

                std::mutex mutex_;
        };
    }
}

#endif
