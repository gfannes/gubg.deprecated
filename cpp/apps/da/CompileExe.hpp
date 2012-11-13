#ifndef HEADER_da_CompileExe_hpp_ALREADY_INCLUDED
#define HEADER_da_CompileExe_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    class CompileExe: public ITask
    {
        public:
            static Ptr create(const std::string &source){return Ptr(new CompileExe(source));}

            virtual ReturnCode execute(const Options &);

        private:
            CompileExe(const std::string &source);

            gubg::file::File source_;
    };
}

#endif
