#ifndef HEADER_pa_Plan_hpp_ALREADY_INCLUDED
#define HEADER_pa_Plan_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class Plan: public ITask
    {
        public:
			enum Level {Overview, Details};

            static ITask::Ptr create(Level level){return ITask::Ptr(new Plan(Level level));}

            virtual ReturnCode execute(const Options &);

        private:
            Plan(Level level):level_(level){}
			const Level level_;
    };
}

#endif
