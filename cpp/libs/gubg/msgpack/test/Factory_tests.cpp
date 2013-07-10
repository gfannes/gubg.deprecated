#include "gubg/msgpack/Factory.hpp"
#include "gubg/Testing.hpp"
#include "gubg/msgpack/test/Helper.hpp"

class Factory: public gubg::msgpack::Factory_crtp<Factory>
{
    public:
        IObject *factory_createObject(unsigned long id)
        {
            return 0;
        }
    private:
};

namespace data
{
    using namespace helper;
    auto msg1 = str_({0x83, 0xc0, 0x00, 0x00, 0xc0, 0x01, 0xc0});
}

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    Factory f;
    f.process(data::msg1);
    return 0;
}
#include "gubg/log/end.hpp"
