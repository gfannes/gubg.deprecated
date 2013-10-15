#include "gubg/msgpack/Builder.hpp"
#include "gubg/FixedVector.hpp"
#include "gubg/Testing.hpp"
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    struct A
    {
        enum {Id = 1};
        template <typename Builder>
            void serialize(Builder &builder) const
            {
                builder.createArray(3).add(1).add(2).add(3);
            }
    };
}
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
        ary.add(A());
        ary.add(123456);
        ary.add(0);
        L(testing::toHex(buffer));
    }
    {
        auto map = builder.createMap(2);
        map.add(1, 2);
        map.add(A());
        L(testing::toHex(buffer));
    }
    TEST_TRUE(builder.valid());
    return 0;
}
#include "gubg/log/end.hpp"
