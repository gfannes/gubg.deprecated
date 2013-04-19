#ifndef HEADER_gubg_parse_numbers_Integer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_numbers_Integer_hpp_ALREADY_INCLUDED

#define GUBG_MODULE "parse::numbers"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace parse
    {
        namespace numbers
        {
            template <typename Int, typename Len>
                bool readInt(Int &i, const char *buf, Len len)
                {
                    SS(buf, len);
                    i = 0;
#define L_TABLE(l,m) const static unsigned long long lt_ ## l[] = {m*0,m*1,m*2,m*3,m*4,m*5,m*6,m*7,m*8,m*9};
                    L_TABLE(1 ,1ul);
                    L_TABLE(2 ,10ul);
                    L_TABLE(3 ,100ul);
                    L_TABLE(4 ,1000ul);
                    L_TABLE(5 ,10000ul);
                    L_TABLE(6 ,100000ul);
                    L_TABLE(7 ,1000000ul);
                    L_TABLE(8 ,10000000ul);
                    L_TABLE(9 ,100000000ul);
                    L_TABLE(10,1000000000ul);
                    switch (len)
                    {
#define L_CASE(l)       case l: i += lt_ ## l[*buf++ - '0']; L(#l ": " << i)
                        L_CASE(10);
                        L_CASE(9);
                        L_CASE(8);
                        L_CASE(7);
                        L_CASE(6);
                        L_CASE(5);
                        L_CASE(4);
                        L_CASE(3);
                        L_CASE(2);
                        L_CASE(1);
                    }
                    return true;
                }
        }
    }
}
#include "gubg/log/end.hpp"

#endif
