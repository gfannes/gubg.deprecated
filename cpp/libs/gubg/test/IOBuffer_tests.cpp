#include "gubg/Testing.hpp"
#include "gubg/IOBuffer.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    typedef gubg::IOBuffer<char> Buffer;
    Buffer buffer(256);
    TEST_EQ(0, buffer.size());
    TEST_EQ(256, buffer.freeSize());
    TEST_TRUE(buffer.empty());
    TEST_FALSE(buffer.full());
    TEST_TRUE(buffer.push_back('a'));
    TEST_FALSE(buffer.empty());
    TEST_FALSE(buffer.full());
    TEST_EQ(1, buffer.size());
    TEST_TRUE(buffer.push_back('b'));
    TEST_EQ(2, buffer.size());
    return 0;
}
#include "gubg/log/end.hpp"
