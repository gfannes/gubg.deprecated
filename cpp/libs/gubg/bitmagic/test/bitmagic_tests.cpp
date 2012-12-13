#include "gubg/testing/Testing.hpp"
#include "gubg/bitmagic/bitmagic.hpp"
using namespace gubg::bitmagic;

int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(rotateLeft);
        TEST_EQ(0x12345678, rotateLeft(0x12345678, 0));
        TEST_EQ(0x2468acf0, rotateLeft(0x12345678, 1));
        TEST_EQ(0x23456781, rotateLeft(0x12345678, 4));
        TEST_EQ(0x34567812, rotateLeft(0x12345678, 8));
        TEST_EQ(0x45678123, rotateLeft(0x12345678, 12));
        TEST_EQ(0x56781234, rotateLeft(0x12345678, 16));
        TEST_EQ(0x67812345, rotateLeft(0x12345678, 20));
        TEST_EQ(0x78123456, rotateLeft(0x12345678, 24));
        TEST_EQ(0x81234567, rotateLeft(0x12345678, 28));
        TEST_EQ(0x12345678, rotateLeft(0x12345678, 32));
    }
    {
        TEST_TAG(rotateRight);
        TEST_EQ(0x12345678, rotateRight(0x12345678, 0));
        TEST_EQ(0x091a2b3c, rotateRight(0x12345678, 1));
        TEST_EQ(0x81234567, rotateRight(0x12345678, 4));
        TEST_EQ(0x78123456, rotateRight(0x12345678, 8));
        TEST_EQ(0x67812345, rotateRight(0x12345678, 12));
        TEST_EQ(0x56781234, rotateRight(0x12345678, 16));
        TEST_EQ(0x45678123, rotateRight(0x12345678, 20));
        TEST_EQ(0x34567812, rotateRight(0x12345678, 24));
        TEST_EQ(0x23456781, rotateRight(0x12345678, 28));
        TEST_EQ(0x12345678, rotateRight(0x12345678, 32));
    }
    return 0;
}
