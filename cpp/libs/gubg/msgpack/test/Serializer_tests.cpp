#include "gubg/msgpack/Serializer.hpp"
#include "gubg/Testing.hpp"
#include <string>
using namespace std;
using namespace gubg::testing;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    class Obj
    {
        public:
            template <typename S>
                bool msgpack_serialize(S &s) const
                {
                    MSS_BEGIN(bool);
                    MSS(s.writeIdAndAttrCnt(0, 2));
                    MSS(s.writeAttribute(0, a_));
                    MSS(s.writeAttribute(1, b_));
                    MSS_END();
                }
        private:
            int a_;
            string b_;
    };
}

int main()
{
    TEST_TAG(main);
    Obj obj;
    gubg::msgpack::Serializer<string> s;
    TEST_OK(s.serialize(obj));
    string buf;
    TEST_OK(s.swap(buf));
    L("Serialized buffer: " << toHex(buf));
    return 0;
}
#include "gubg/log/end.hpp"
