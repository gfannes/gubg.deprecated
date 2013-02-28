#include "gubg/msgpack/Builder.hpp"
#include "gubg/FixedVector.hpp"
#include "gubg/l.hpp"
#include "gubg/Testing.hpp"
using namespace gubg;

int main()
{
    typedef FixedVector<char, 1024> Buffer;
    typedef msgpack::Array<Buffer> Array;
    typedef msgpack::Map<Buffer> Map;

    Buffer buffer;
    Array ary(buffer, 4);
    ary.push(12);
    ary.push(34);
    ary.push(123456);
    ary.push(0);
    L(testing::toHex(buffer));
    Map map(buffer, 2);
    map.push(1, 2);
    map.push(4, 24);
    L(testing::toHex(buffer));
    return 0;
}
