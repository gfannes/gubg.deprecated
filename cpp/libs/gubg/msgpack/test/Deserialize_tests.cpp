#include "gubg/msgpack/Deserialize.hpp"
#include "gubg/Testing.hpp"
#include <string>
using namespace gubg::msgpack;
using namespace std;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    struct A
    {
        void msgpack_set(AttributeId aid, long v) {S();L(STREAM(aid, v));}
        void msgpack_set(AttributeId aid, Nil_tag) {S();L(STREAM(aid) << " nil");}
        void msgpack_set(AttributeId aid, const string &v) {S();L(STREAM(aid, v));}
    };
}
int main()
{
    TEST_TAG(main);
    A a;
    Object_itf<string> *obj = wrap<string>(a);
    obj->set(0, 0);
    return 0;
}
#include "gubg/log/end.hpp"
