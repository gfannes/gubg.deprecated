#ifndef gubg_parse_BreakUp_hpp
#define gubg_parse_BreakUp_hpp

//breakUp() implements a O(n) algorithm for breaking-up a range (begin, end)
//Breaker.operator()(b, m, e) will be called repeatedly, if it returns true
//breakUp() assumes (b, m) is accepted as a token and thus consumed. As such,
//the next call of Breaker.operator()() will start from (m, m+1, e)

namespace gubg
{
    namespace parse
    {
        template <typename Iterator, typename Breaker>
            void breakUp(Iterator begin, Iterator end, Breaker &breaker)
            {
                if (begin == end)
                    return;
                Iterator mid = begin;
                while (++mid != end)
                {
                    if (breaker(begin, mid, end))
                        begin = mid;
                }
                breaker(begin, mid, end);
            }
    }
}

#endif
