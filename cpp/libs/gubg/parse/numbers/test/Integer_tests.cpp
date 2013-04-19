#include "gubg/parse/numbers/Integer.hpp"
#include "gubg/Testing.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    auto a = "1234567890";
    auto aa = 1234567890;
    unsigned long long bb = 0;
    TEST_TRUE(gubg::parse::numbers::readInt(bb, a, 10));
    TEST_EQ(aa, bb);
    {
        TEST_TAG(stress);
        auto Nr = 10000000;
        {
            SSS("mine");
            for (unsigned long i = 0; i < Nr; ++i)
                gubg::parse::numbers::readInt(bb, a, 10);
        }
        {
            SSS("strtol");
            char *e;
            for (unsigned long i = 0; i < Nr; ++i)
                bb = std::strtol(a, &e, 10);
        }
    }
    return 0;
}
#include "gubg/log/end.hpp"
