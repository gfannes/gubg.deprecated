#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/Testing.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include <string>
using namespace std;
using namespace gubg;
using namespace gubg::testing;

int main()
{
    TEST_TAG(main);
    msgpack::Type type;
    TEST_OK(type.read(0xc0));
    TEST_TRUE(msgpack::Primitive::Nil == type.primitive);
    TEST_TRUE(msgpack::Group::Nil == type.group);
    TEST_EQ(0, type.nr);
    TEST_OK(type.read(0xc2));
    TEST_TRUE(msgpack::Primitive::Boolean == type.primitive);
    TEST_TRUE(msgpack::Group::Boolean == type.group);
    TEST_EQ(0, type.nr);
    TEST_OK(type.read(0x01));
    TEST_EQ(1, type.nr);
    return 0;
}