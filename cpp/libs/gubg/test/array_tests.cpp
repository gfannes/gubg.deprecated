#include "gubg/array.hpp"
#include "gubg/Testing.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    gubg::array<uint8_t, 10> ary;
    for (auto it = ary.begin(); it != ary.end(); ++it)
        TEST_EQ(0, *it);
    return 0;
}
#include "gubg/log/end.hpp"
