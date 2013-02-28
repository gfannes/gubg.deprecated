#include "gubg/msgpack/Builder.hpp"
#include "gubg/FixedVector.hpp"
#include "gubg/l.hpp"
#include "gubg/Testing.hpp"
using namespace gubg;

int main()
{
    TEST_TAG(main);
    typedef FixedVector<char, 1024> Buffer;
    typedef msgpack::Builder<Buffer> Builder;

    Buffer buffer;
    Builder builder(buffer);
    {
        auto ary = builder.createArray(4);
        ary.add(12);
        ary.add(34);
        ary.add(123456);
        ary.add(0);
        L(testing::toHex(buffer));
    }
    {
        auto map = builder.createMap(2);
        map.add(1, 2);
        map.add(4, 24);
        L(testing::toHex(buffer));
    }
    TEST_TRUE(builder.valid());
    return 0;
}
