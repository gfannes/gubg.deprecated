#include "gubg/msgpack/Factory.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/Testing.hpp"
#include "gubg/msgpack/test/Helper.hpp"
#include <iostream>

struct Ids
{
};

#define GUBG_MODULE_ "Work"
#include "gubg/log/begin.hpp"
struct Work
{
    std::string nonce;
    std::string msg;
    void stream(std::ostream &os) const
    {
        os << STREAM(nonce, msg) << std::endl;
    }
    template <typename Buffer>
        bool msgpack_serialize(Buffer &buffer)
        {
            MSS_BEGIN(bool);
            MSS(writeIdAndLength(buffer, 0, 2));
            MSS(writeMember(buffer, 0, nonce));
            MSS(writeMember(buffer, 1, msg));
            MSS_END();
        }
    void msgpack_set(gubg::msgpack::AttributeId id, gubg::msgpack::Nil_tag)
    {
        S();L("Setting member " << id << " to nil");
        switch (id)
        {
            case 0:
                nonce.clear();
                break;
            case 1:
                msg.clear();
                break;
        }
    }
    void msgpack_set(gubg::msgpack::AttributeId id, const std::string &str)
    {
        S();L("Setting member " << id << " to str");
        switch (id)
        {
            case 0:
                nonce.assign(&str[0], str.size());
                break;
            case 1:
                msg.assign(&str[0], str.size());
                break;
        }
    }
    void msgpack_set(gubg::msgpack::AttributeId id, long v)
    {
        S();L("Setting member " << id << " to " << v);
    }
};
Work work;
#include "gubg/log/end.hpp"

enum class ReturnCode {MSS_DEFAULT_CODES,};

#define GUBG_MODULE_ "test_Factory"
#include "gubg/log/begin.hpp"
class Factory: public gubg::msgpack::Factory_crtp<Factory, std::string, 15>
{
    public:
        gubg::msgpack::Wrapper<std::string> msgpack_createObject(AttributeId aid, TypeId tid)
        {
            SS(aid, tid);
            switch (tid)
            {
                case 0: return wrap(work);
            }
            return gubg::msgpack::Wrapper<std::string>();
        }
        void msgpack_createdObject(AttributeId aid, TypeId tid)
        {
            SS(aid, tid);
        }
        void msgpack_set(AttributeId id, gubg::msgpack::Nil_tag) {S();L("???");}
        void msgpack_set(AttributeId id, const std::string &str) {S();L("???");}
        void msgpack_set(AttributeId id, long) {S();L("???");}
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
    gubg::msgpack::Serializer<std::string, Ids, 10> serializer;
    serializer.serialize(buffer);
    std::cout << gubg::testing::toHex(buffer) << std::endl;
    return 0;
}
#include "gubg/log/end.hpp"