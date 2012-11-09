#ifndef HEADER_gubg_parse_BreakUp_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_BreakUp_hpp_ALREADY_INCLUDED

//breakUp() implements a O(n) algorithm for breaking-up a range (start, end)
//Consumer.operator()(s, m, e) will be called repeatedly, if it returns true
//breakUp() assumes (s, m) is accepted as a token and thus consumed. As such,
//the next call of Consumer.operator()() will start from (m, m+1, e)
//Consumer can detect the end in mid == end. The last return value of consumer
//is also the return value of breakUp

namespace gubg
{
    namespace parse
    {
        template <typename Iterator, typename Consumer>
            bool breakUp(Iterator start, Iterator end, Consumer &consumer)
            {
                if (start == end)
                    return false;
                Iterator mid = start;
                while (++mid != end)
                {
                    if (consumer(start, mid, end))
                        start = mid;
                }
                return consumer(start, mid, end);
            }
    }
}

#endif
