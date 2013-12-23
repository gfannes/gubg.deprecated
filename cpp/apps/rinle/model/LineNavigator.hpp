#ifndef HEADER_rinle_model_LineNavigator_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_LineNavigator_hpp_ALREADY_INCLUDED

#include "rinle/model/Types.hpp"
#include "rinle/model/Navigator.hpp"
#include <cassert>

#define GUBG_MODULE_ "LineNavigator"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model {

    class LineNavigator: public Navigator
    {
        public:
			LineNavigator(Tokens &tokens): Navigator(tokens) {}

			//Navigator API
            virtual Range set(const Range &range) const
			{
				S();

				auto info = infoPerToken_.find(&range.front());
				if (info == infoPerToken_.end())
				{
					L("Nothing known about the front of this range");
					return Range();
				}

				Range ret;
				//We set a failsafe end for the range at first
				ret = Range(info->second.begin, tokens_.end());

				//We try to find a better end for the range: proceed the end of the previous range
				//to the next line
				for (auto betterEnd = range.end(); betterEnd != tokens_.end(); ++betterEnd)
				{
					if (betterEnd->token.type != gubg::parse::cpp::pp::Token::LineFeed)
						continue;
					//We found a more suitable end for the range
					ret.end(++betterEnd);
					break;
				}
				return ret;
			}
			virtual bool move(Range &range, Direction dir) const
			{
				switch (dir)
				{
					case Forward: return forward(range); break;
					case Backward: return backward(range); break;
				}
				return false;
			}

            void refresh()
            {
                recompute_();
            }

            //ix is zero-based, so it is _not_ the line number
            Range getLine(size_t *ix, Range::Iterator token) const
            {
                auto info = infoPerToken_.find(&*token);
                if (info == infoPerToken_.end())
                    return Range();
				auto lix = info->second.ix;
				if (ix)
					*ix = lix;
				return getLine(lix);
            }
			Range getLine(size_t ix) const
			{
				if (ix >= lines_.size())
					return Range();
				return lines_[ix];
			}

			bool forward(Range &range) const
			{
                S();
				size_t ix;
				if (getLine(&ix, range.begin()).empty())
					return false;
				if (ix+1 >= lines_.size())
					return false;
				range = lines_[ix+1];
				return true;
			}
			bool backward(Range &range) const
			{
                S();
				size_t ix;
				if (getLine(&ix, range.begin()).empty())
					return false;
				if (ix == 0)
					return false;
				range = lines_[ix-1];
				return true;
			}

		private:
			void recompute_()
			{
				S();
				infoPerToken_.clear();
				size_t ix = 0;
				auto lineStart = tokens_.begin();
				lines_.push_back(Range(lineStart, lineStart));
				for (auto it = tokens_.begin(); it != tokens_.end(); ++it)
				{
					infoPerToken_[&*it] = Info({ix, lineStart});
					if (it->token.type == gubg::parse::cpp::pp::Token::LineFeed)
					{
						lineStart = std::next(it);
						lines_.back().end(lineStart);
						lines_.push_back(Range(lineStart, lineStart));
						++ix;
					}
				}
				lines_.back().end(tokens_.end());
				if (lines_.back().empty())
					lines_.pop_back();
				L(STREAM(infoPerToken_.size()));
			}

			struct Info
			{
				size_t ix;
				Tokens::iterator begin;
			};
			typedef std::map<OrderedToken*, Info> InfoPerToken;
			InfoPerToken infoPerToken_;
			typedef std::vector<Range> Lines;
			Lines lines_;
	};

} }
#include "gubg/log/end.hpp"

#endif
