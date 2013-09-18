#ifndef HEADER_da_link_Linker_hpp_ALREADY_INCLUDED
#define HEADER_da_link_Linker_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/Types.hpp"
#include "gubg/file/File.hpp"
#include "gubg/Verbose.hpp"
#include <string>
#include <list>
#include <mutex>

namespace da
{
    namespace compile
    {
        class Linker: public gubg::Verbose<true>
        {
            public:
                Linker(ExeType);

                LinkSettings settings;
                void runAfterLinking(bool b) {doRun_ = b;}

                typedef gubg::file::File ExeFile;
                typedef gubg::file::File ObjectFile;
                typedef std::list<ObjectFile> ObjectFiles;
                ReturnCode operator()(const ExeFile &, const ObjectFiles &);

            private:
                std::mutex mutex_;
                const ExeType exeType_;
                bool doRun_;
        };
    }
}

#endif
