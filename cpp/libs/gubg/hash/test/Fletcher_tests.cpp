#include "gubg/hash/Fletcher16.hpp"
#include "gubg/Testing.hpp"

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    L(std::hex << gubg::hash::fletcher16((const uint8_t *)"abcd", 4));
    L(std::hex << gubg::hash::fletcher16((const uint8_t *)"abce", 4));
    L(std::hex << gubg::hash::fletcher16((const uint8_t *)"bbce", 4));
    return 0;
}
#include "gubg/log/end.hpp"
