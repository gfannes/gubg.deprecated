#include "gubg/range/Concept.hpp"
#include "gubg/Testing.hpp"
#include <iostream>

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);

    size_t sum = 0;
    const size_t Nr = 10000000;
    for (size_t i = 0; i < Nr; ++i)
        sum += gubg::range::g();
    std::cout << sum << std::endl;

    return 0;
}
#include "gubg/log/end.hpp"
