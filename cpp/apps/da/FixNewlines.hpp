#ifndef HEADER_da_FixNewlines_hpp_ALREADY_INCLUDED
#define HEADER_da_FixNewlines_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"
#include "gubg/parse/Line.hpp"

namespace da
{
    class FixNewlines: public Task_itf
    {
        public:
            static Ptr create(gubg::line::Line::End end){return Ptr(new FixNewlines(end));}
            virtual ReturnCode execute(const Options &);

		private:
			FixNewlines(gubg::line::Line::End wantedEnd): wantedEnd_(wantedEnd) {}

			const gubg::line::Line::End wantedEnd_;
    };
}

#endif
