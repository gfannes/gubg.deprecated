#include "gubg/Testing.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include <string>
using namespace std;
using namespace gubg;
using namespace gubg::testing;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    msgpack::Type type;

	//nil
    TEST_OK(type.read(0xc0));
    TEST_TRUE(msgpack::Primitive::Nil == type.primitive);
    TEST_TRUE(msgpack::Group::Nil == type.group);
    TEST_EQ(0, type.nr);

	//false
    TEST_OK(type.read(0xc2));
    TEST_TRUE(msgpack::Primitive::False == type.primitive);
    TEST_TRUE(msgpack::Group::Boolean == type.group);
    TEST_EQ(0, type.nr);

	//true
    TEST_OK(type.read(0xc3));
    TEST_TRUE(msgpack::Primitive::True == type.primitive);
    TEST_TRUE(msgpack::Group::Boolean == type.group);
    TEST_EQ(1, type.nr);

	//1
    TEST_OK(type.read(0x01));
    TEST_EQ(1, type.nr);

    return 0;
}
#include "gubg/log/end.hpp"
