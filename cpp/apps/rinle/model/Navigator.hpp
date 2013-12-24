#ifndef HEADER_rinle_model_Navigator_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Navigator_hpp_ALREADY_INCLUDED

#include "rinle/model/Types.hpp"

namespace rinle
{
	namespace model
	{
		class Navigator
		{
			public:
				virtual bool set(Range &) const = 0;
				virtual bool move(Range &, Direction) = 0;	

			protected:
				Navigator(Tokens &tokens): tokens_(tokens) {}

				Tokens &tokens_;
		};
	}
}

#endif
