#include "gubg/msgpack/Factory.hpp"
#include "gubg/Testing.hpp"
#include "gubg/msgpack/test/Helper.hpp"

#define GUBG_MODULE_ "Factory"
#include "gubg/log/begin.hpp"
class Factory: public gubg::msgpack::Factory_crtp<Factory>
{
    public:
        IObject *factory_createObject(unsigned long id)
        {
            SS(id);
            return 0;
        }
        void factory_primitive(unsigned long l)
        {
            SS(l);
        }
    private:
};
#include "gubg/log/end.hpp"

namespace data
{
    using namespace helper;
    auto msg_0 = str_({0x00});
    auto msg_ut = str_({0x83, 0xc0, 0x00, 0x00, 0xc0, 0x01, 0xc0});
}

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    Factory f;
    f.process(data::msg_0);
    f.process(data::msg_ut);
    return 0;
}
#include "gubg/log/end.hpp"
