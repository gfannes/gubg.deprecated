#include "gubg/msgpack/MsgPack.hpp"
#include <string>
using namespace std;
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    MSS_BEGIN(int);
    msgpack::Buffer<string> buffer;
    MSS_END();
}
#include "gubg/log/end.hpp"
