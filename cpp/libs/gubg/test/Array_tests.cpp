#include "gubg/Array.hpp"
#include "gubg/Testing.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    auto ary = gubg::make_array(1, 2, 3);
    TEST_EQ(3, ary.size());
    return 0;
}
#include "gubg/log/end.hpp"
