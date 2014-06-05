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
		template <typename Wrapper>
			void msgpack_createObject(Wrapper &, RoleId) {}
        void msgpack_set(RoleId rid, long v) {S();L(STREAM(rid, v));}
        void msgpack_set(RoleId rid, Nil_tag) {S();L(STREAM(rid) << " nil");}
        void msgpack_set(RoleId rid, const string &v) {S();L(STREAM(rid, v));}
		void msgpack_createdObject(RoleId) {}
    };
}
int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(defctor);
        Wrapper<string> wrapper;
        TEST_FALSE(wrapper.isValid());
        TEST_TRUE(&wrapper == &(wrapper = Wrapper<string>()));
    }
    A a;
    Wrapper<string> wrapper(a);
    TEST_TRUE(wrapper.isValid());
    wrapper.set(0, 0);
    Wrapper<string> w2 = wrapper;
    w2.set(1, string("oeuaou"));
    w2.set(2, 0);
    return 0;
}
#include "gubg/log/end.hpp"
