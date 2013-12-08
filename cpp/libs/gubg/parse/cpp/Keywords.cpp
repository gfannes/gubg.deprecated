#include "gubg/parse/cpp/Keywords.hpp"

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            bool isKeyword(const std::string &str)
            {
#define CHECK_KEYWORD(kw) if (str == #kw) return true
                CHECK_KEYWORD(class);
                CHECK_KEYWORD(struct);
                CHECK_KEYWORD(namespace);
                CHECK_KEYWORD(enum);
                CHECK_KEYWORD(union);

                CHECK_KEYWORD(true);
                CHECK_KEYWORD(false);
                CHECK_KEYWORD(nullptr);

                CHECK_KEYWORD(bool);
                CHECK_KEYWORD(char);
                CHECK_KEYWORD(short);
                CHECK_KEYWORD(int);
                CHECK_KEYWORD(long);
                CHECK_KEYWORD(float);
                CHECK_KEYWORD(double);

                CHECK_KEYWORD(unsigned);
                CHECK_KEYWORD(const);
                CHECK_KEYWORD(static);
                CHECK_KEYWORD(volatile);
                CHECK_KEYWORD(inline);

                CHECK_KEYWORD(auto);

                CHECK_KEYWORD(if);
                CHECK_KEYWORD(for);
                CHECK_KEYWORD(while);

                CHECK_KEYWORD(return);

                CHECK_KEYWORD(catch);

                return false;
            }
        }
    }
}
