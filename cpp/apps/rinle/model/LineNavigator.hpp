#ifndef HEADER_rinle_model_LineNavigator_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_LineNavigator_hpp_ALREADY_INCLUDED

#include "rinle/model/Types.hpp"
#include <cassert>

#define GUBG_MODULE_ "LineNavigator"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model {

    class LineNavigator
    {
        public:
            LineNavigator(Tokens &tokens)
                : tokens_(tokens) {}

            void refresh()
            {
                recompute_();
            }

            //ix is zero-based, so it is _not_ the line number
            bool getLineIX(size_t &ix, Tokens::iterator token) const
            {
                auto info = infoPerToken_.find(&*token);
                if (info == infoPerToken_.end())
                    return false;
                ix = info->second.ix;
                return true;
            }

            Range set(Range range) const
            {
                S();
                Range ret;
                auto info = infoPerToken_.find(&*range.begin);
                if (info != infoPerToken_.end())
                {
                    ret.begin = info->second.begin;
                    ret.end = range.end;
                    if (!forward_(ret.end))
                        ret.end = tokens_.end();
                }
                return ret;
            }

            bool forward(Range &range) const
            {
                size_t ix;
                if (!getLineIX(ix, range.begin))
                    return false;
                if (ix+1 >= lines_.size())
                    return false;
                range = lines_[ix+1];
                return true;
            }
            bool backward(Range &range) const
            {
                size_t ix;
                if (!getLineIX(ix, range.begin))
                    return false;
                if (ix == 0)
                    return false;
                range = lines_[ix-1];
                return true;
            }

        private:
            bool forward_(Tokens::iterator &it) const
            {
                for (; it != tokens_.end(); ++it)
                {
                    if (it->type == gubg::parse::cpp::pp::Token::LineFeed)
                    {
                        it = std::next(it);
                        return true;
                    }
                }
                return false;
            }
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
                    if (it->type == gubg::parse::cpp::pp::Token::LineFeed)
                    {
                        lineStart = std::next(it);
                        lines_.back().end = lineStart;
                        lines_.push_back(Range(lineStart, lineStart));
                        ++ix;
                    }
                }
                lines_.back().end = tokens_.end();
                if (lines_.back().empty())
                    lines_.pop_back();
                L(STREAM(infoPerToken_.size()));
            }

            Tokens &tokens_;
            struct Info
            {
                size_t ix;
                Tokens::iterator begin;
            };
            typedef std::map<Token*, Info> InfoPerToken;
            InfoPerToken infoPerToken_;
            typedef std::vector<Range> Lines;
            Lines lines_;
    };

} }
#include "gubg/log/end.hpp"

#endif
