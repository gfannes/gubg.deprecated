#ifndef HEADER_rinle_model_TokenNavigator_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_TokenNavigator_hpp_ALREADY_INCLUDED

#include "rinle/model/Navigator.hpp"

namespace rinle
{
	namespace model
	{
		class TokenNavigator: public Navigator
		{
			public:
				TokenNavigator(Tokens &tokens): Navigator(tokens) {}
				virtual Range set(const Range &range) const
				{
					return Range();
				}
				virtual bool move(Range &range, Direction dir) const
				{
					return false;
				}
			private:
		};
	}
}

#endif
