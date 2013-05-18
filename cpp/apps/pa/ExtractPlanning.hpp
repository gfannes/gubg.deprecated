#ifndef HEADER_pa_ExtractPlanning_hpp_ALREADY_INCLUDED
#define HEADER_pa_ExtractPlanning_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class ExtractPlanning: public ITask
    {
        public:
            static Ptr create(const std::string &file){return Ptr(new ExtractPlanning(file));}

            virtual ReturnCode execute(const Options &);

        private:
            ExtractPlanning(const std::string &file);

            gubg::file::File file_;
    };
}

#endif
