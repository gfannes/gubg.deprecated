#ifndef HEADER_da_CompileExe_hpp_ALREADY_INCLUDED
#define HEADER_da_CompileExe_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"
#include "da/Types.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    class CompileExe: public Task_itf
    {
        public:
            static Ptr create(const std::string &source, ExeType exeType){return Ptr(new CompileExe(source, exeType));}

            virtual ReturnCode execute(const Options &);

        private:
            CompileExe(const std::string &source, ExeType);

            gubg::file::File source_;
            const ExeType exeType_;
    };
}

#endif
