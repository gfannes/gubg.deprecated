#ifndef HEADER_rinle_view_Layout_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Layout_hpp_ALREADY_INCLUDED

#include "gubg/Platform.hpp"

namespace rinle
{
    namespace view
    {
		struct Layout
		{
#ifdef GUBG_MINGW
			const int width = 1580;
			const int height = 1100;
			const int nrRows = 40;
			const int focusRow = 15;
#else
			const int width = 500;
			const int height = 300;
			const int nrRows = 15;
			const int focusRow = 15;
#endif
			const int fontSize = 14;
			const char *font = "Consolas";
		};
    }
}

#endif
