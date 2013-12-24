#ifndef HEADER_rinle_model_TokenNavigator_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_TokenNavigator_hpp_ALREADY_INCLUDED

#include "rinle/model/Navigator.hpp"

#define GUBG_MODULE_ "TokenNavigator"
#include "gubg/log/begin.hpp"
namespace rinle
{
	namespace model
	{
		class TokenNavigator: public Navigator
		{
			public:
				TokenNavigator(Tokens &tokens): Navigator(tokens) {}
				virtual bool set(Range &range) const
				{
					S();
					if (range.empty())
					{
						if (tokens_.empty())
						{
							L("This document is empty");
							return false;
						}
						L("Don't know what to do, giving the first token");
						range = Range(tokens_.begin(), std::next(tokens_.begin()));
						return true;
					}
					L("Starting from a non-empty range");
					range = Range(range.begin(), std::next(range.begin()));
					return true;
				}
				virtual bool move(Range &range, Direction dir)
				{
					if (range.empty())
						return false;
					switch (dir)
					{
						case Forward:
							{
								auto newBegin = std::next(range.begin());
								if (newBegin == tokens_.end())
									return false;
								range = Range(newBegin, std::next(newBegin));
							}
							break;
						case Backward:
							{
								if (range.begin() == tokens_.begin())
									return false;
								auto newBegin = std::prev(range.begin());
								range = Range(newBegin, range.begin());
							}
							break;
					}
					return true;
				}
		};
	}
}
#include "gubg/log/end.hpp"

#endif
