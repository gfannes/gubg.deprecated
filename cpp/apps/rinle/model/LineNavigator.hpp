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
            virtual bool set(Range &range) const
			{
				S();

				if (range.empty())
				{
					if (lines_.empty())
					{
						L("This is an empty document");
						return false;
					}
					range = lines_.front();
					return true;
				}

				auto info = infoPerToken_.find(&range.front());
				if (info == infoPerToken_.end())
				{
					L("Nothing known about the front of this range");
					range = lines_.front();
					return true;
				}

				range = Range(info->second.begin, info->second.begin);
				size_t size = size_;
				for (; range.end() != tokens_.end(); range.end(std::next(range.end())))
				{
					if (range.end()->token.type != gubg::parse::cpp::pp::Token::LineFeed)
						continue;
					L("Decreasing size from " << size);
					--size;
					if (size == 0)
					{
						L("We found the end of the range");
						range.end(std::next(range.end()));
						break;
					}
				}
				return true;
			}
			virtual bool move(Range &range, Direction dir)
			{
				switch (dir)
				{
					case Forward:  return forward_(range); break;
					case Backward: return backward_(range); break;
					case Out:
								   size_ *= 2;
								   return set(range);
								   break;
					case In:
								   if (size_ == 1)
									   return false;
								   size_ /= 2;
								   return set(range);
								   break;
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
			bool forward_(Range &range) const
			{
                S();
				size_t ix;
				if (getLine(&ix, range.begin()).empty())
					return false;
				assert(!lines_.empty());
				ix += size_;
				if (ix >= lines_.size())
					ix = lines_.size()-1;

				auto ixe = ix+size_-1;
				if (ixe >= lines_.size())
					ixe = lines_.size()-1;
				range.begin(lines_[ix].begin());
				range.end(lines_[ixe].end());
				return true;
			}
			bool backward_(Range &range) const
			{
                S();
				size_t ix;
				if (getLine(&ix, range.begin()).empty())
					return false;
				assert(!lines_.empty());
				if (size_ >= ix)
					ix = 0;
				else
					ix -= size_;

				auto ixe = ix+size_-1;
				if (ixe >= lines_.size())
					ixe = lines_.size()-1;
				range.begin(lines_[ix].begin());
				range.end(lines_[ixe].end());
				return true;
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
			size_t size_ = 1;
	};

} }
#include "gubg/log/end.hpp"

#endif
