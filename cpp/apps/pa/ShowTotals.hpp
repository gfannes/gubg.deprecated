#ifndef HEADER_pa_ShowTotals_hpp_ALREADY_INCLUDED
#define HEADER_pa_ShowTotals_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class ShowTotals: public ITask
    {
        public:
            static Ptr create(const std::string &name){return Ptr(new ShowTotals(name));}

            virtual ReturnCode execute(const Options &);

        private:
            ShowTotals(const std::string &name);

            std::string name_;
    };
}

#endif
