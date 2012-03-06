#include "d9/io.hpp"
#include "gubg/testing/Testing.hpp"
using namespace gubg::coding::d9;

int main()
{
    TEST_TAG(io);
    ubyte b;
    {
        TEST_TAG(FromByte);
        Input<FromByte> input(0xd9);
        TEST_OK(input.read(b));
        TEST_EQ(0xd9, b);
        TEST_KO(input.read(b));
    }
    {
        TEST_TAG(FromString);
        string str("abc");
        Input<FromString> input(str);
        TEST_OK(input.read(b));
        TEST_EQ('a', b);
        TEST_OK(input.read(b));
        TEST_EQ('b', b);
        TEST_OK(input.read(b));
        TEST_EQ('c', b);
        TEST_KO(input.read(b));
    }
    {
        TEST_TAG(FromStream);
        istringstream str("abc");
        Input<FromStream> input(str);
        TEST_OK(input.read(b));
        TEST_EQ('a', b);
        TEST_OK(input.read(b));
        TEST_EQ('b', b);
        TEST_OK(input.read(b));
        TEST_EQ('c', b);
        TEST_KO(input.read(b));
    }
    return 0;
}
