#include "gubg/msgpack/Factory.hpp"
#include "gubg/Testing.hpp"
#include "gubg/msgpack/test/Helper.hpp"

struct Work
{
    std::string nonce;
    std::string msg;
};
Work work;

#define GUBG_MODULE_ "test_Factory"
#include "gubg/log/begin.hpp"
class Factory: public gubg::msgpack::Factory_crtp<Factory>
{
    public:
        IObject *factory_createObject(unsigned long id)
        {
            SS(id);
            switch (id)
            {
                case 0: return wrap(work);
            }
            return 0;
        }
        void factory_primitive(unsigned long l)
        {
            SS(l);
        }
        void factory_primitive(gubg::msgpack::Nil_tag)
        {
            S();
            L("nil");
        }
        void factory_setMember(Work &work, unsigned long id, gubg::msgpack::Nil_tag)
        {
            switch (id)
            {
                case 0:
                    work.nonce.clear();
                    break;
                case 1:
                    work.msg.clear();
                    break;
            }
        }
    private:
};
#include "gubg/log/end.hpp"

namespace data
{
    using namespace helper;
    auto msg_0 = str_({0x00});
    auto msg_nil = str_({0xc0});
    auto msg_ut = str_({0x83, 0xc0, 0x00, 0x00, 0xa3, 0x31, 0x32, 0x33, 0x01, 0xc0});
}

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    Factory f;
//    f.process(data::msg_0);
//    f.process(data::msg_nil);
    f.process(data::msg_ut);
    return 0;
}
#include "gubg/log/end.hpp"
