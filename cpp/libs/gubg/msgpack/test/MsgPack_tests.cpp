#include "gubg/msgpack/MsgPack.hpp"
#include <string>
using namespace std;
using namespace gubg;

int main()
{
    MSS_BEGIN(int);
    msgpack::Buffer<string> buffer;
    MSS_END();
}
