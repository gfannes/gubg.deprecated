#ifndef HEADER_rinle_view_Layout_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Layout_hpp_ALREADY_INCLUDED

#include "gubg/Platform.hpp"

namespace rinle
{
    namespace view
    {
		namespace layout
		{
#ifdef GUBG_MINGW
			const int Width = 1580;
			const int Height = 1100;
			const int FontSize = 14;
			const int NrRows = 40;
			const int FocusRow = 15;
#else
			const int Width = 1365;
			const int Height = 800;
			const int FontSize = 14;
			const int NrRows = 30;
			const int FocusRow = 15;
#endif
		}
    }
}

#endif
