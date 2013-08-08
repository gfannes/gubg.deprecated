#ifndef HEADER_pa_Plan_hpp_ALREADY_INCLUDED
#define HEADER_pa_Plan_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"

namespace pa
{
    class Plan: public ITask
    {
        public:
			enum Level {Overview, Details};

            static Ptr create(Level level){return Ptr(new Plan(level));}

            virtual ReturnCode execute(const Options &);

        private:
            Plan(Level level):level_(level){}
			const Level level_;
    };
}

#endif
