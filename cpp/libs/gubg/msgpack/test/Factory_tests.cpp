#include "gubg/msgpack/Factory.hpp"
#include "gubg/Testing.hpp"
#include "gubg/msgpack/test/Helper.hpp"
#include <iostream>

struct Work
{
    std::string nonce;
    std::string msg;
    void stream(std::ostream &os) const
    {
        os << STREAM(nonce, msg) << std::endl;
    }
};
Work work;

enum class ReturnCode {MSS_DEFAULT_CODES,};

#define GUBG_MODULE_ "test_Factory"
#include "gubg/log/begin.hpp"
class Factory: public gubg::msgpack::Factory_crtp<Factory, 10>
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
        void factory_primitive(const String &str)
        {
            SS(str.size());
        }
        void factory_primitive(gubg::msgpack::Nil_tag)
        {
            S();
            L("nil");
        }
        void factory_setMember(Work &work, unsigned long id, gubg::msgpack::Nil_tag)
        {
            S();L("Setting member " << id << " to nil");
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
        void factory_setMember(Work &work, unsigned long id, const String &str)
        {
            S();L("Setting member " << id << " to str");
            switch (id)
            {
                case 0:
                    work.nonce.assign(&str[0], str.size());
                    break;
                case 1:
                    work.msg.assign(&str[0], str.size());
                    break;
            }
        }
        template <typename Buffer>
            ReturnCode factory_serialize(Buffer &buffer, const Work &work)
            {
                MSS_BEGIN(ReturnCode);
                MSS(writeIdAndLength(buffer, 0, 2));
                MSS(writeMember(buffer, 0, work.nonce));
                MSS(writeMember(buffer, 1, work.msg));
                MSS_END();
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
    work.stream(std::cout);

    std::string buffer;
    f.serialize(buffer, work);
    std::cout << gubg::testing::toHex(buffer) << std::endl;
    return 0;
}
#include "gubg/log/end.hpp"
