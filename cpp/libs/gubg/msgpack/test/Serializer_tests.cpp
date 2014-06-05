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
            template <typename Serializer>
                bool msgpack_serialize(Serializer &s) const
                {
                    MSS_BEGIN(bool);
                    auto c = s.createComposer(2);
                    MSS(c.ok());
                    MSS(c.put(a__, a_));
                    MSS(c.put(b__, b_));
                    MSS(c.full());
                    MSS_END();
                }
        private:
            enum {a__, b__};
            int a_ = 0x12345678;
            string b_ = "abc";
    };

    struct Ids
    {
        enum {Start = 123, Obj};
    };
}

int main()
{
    TEST_TAG(main);
    Obj obj;
    gubg::msgpack::Serializer<string, Ids, 15> s;
    TEST_OK(s.serialize(obj));
    string buf;
    TEST_OK(s.swap(buf));
    L("Serialized buffer: " << toHex(buf));
    return 0;
}
#include "gubg/log/end.hpp"
