#include "gubg/msgpack/Factory.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/Testing.hpp"
#include "gubg/msgpack/test/Helper.hpp"
#include <iostream>

#define GUBG_MODULE_ "Work"
#include "gubg/log/begin.hpp"
struct Work
{
	enum {id_rid, nonce_rid, msg_rid, flag_rid, nr_};
	int id;
    std::string nonce;
    std::string msg;
	bool flag = false;

    void stream(std::ostream &os) const
    {
        os << STREAM(id, nonce, msg, flag) << std::endl;
    }
    template <typename Serializer>
        bool msgpack_serialize(Serializer &s)
        {
			MSS_BEGIN(bool);
			auto c = s.createComposer(nr_);
			MSS(c.ok());
			MSS(c.put(id_rid, id));
			MSS(c.put(nonce_rid, nonce));
			MSS(c.put(msg_rid, msg));
			MSS(c.put(flag_rid, flag));
			MSS(c.full());
			MSS_END();
        }

	typedef gubg::msgpack::RoleId RoleId;
	template <typename Wrapper>
		void msgpack_createObject(Wrapper &obj, RoleId rid)
		{
			S();L("Creating object " << STREAM(rid));
		}
	void msgpack_createdObject(RoleId rid)
	{
		S();L("Created object " << STREAM(rid));
	}
    void msgpack_set(gubg::msgpack::RoleId rid, gubg::msgpack::Nil_tag)
    {
        S();L("Setting " << STREAM(rid) << " to nil");
        switch (rid)
        {
            case id_rid: id = 0; break;
            case nonce_rid: nonce.clear(); break;
            case msg_rid: msg.clear(); break;
            case flag_rid: flag = false; break;
        }
    }
    void msgpack_set(gubg::msgpack::RoleId rid, const std::string &str)
    {
        S();L("Setting " << STREAM(rid) << " to " << str);
        switch (rid)
        {
            case nonce_rid: nonce.assign(&str[0], str.size()); break;
            case msg_rid: msg.assign(&str[0], str.size()); break;
        }
    }
    void msgpack_set(gubg::msgpack::RoleId rid, long v)
    {
        S();L("Setting " << STREAM(rid) << " to long " << v);
        switch (rid)
        {
            case id_rid: id = v; break;
        }
    }
    void msgpack_set(gubg::msgpack::RoleId rid, bool b)
    {
        S();L("Setting " << STREAM(rid) << " to bool " << (bool)b);
        switch (rid)
        {
            case flag_rid: flag = b; break;
        }
    }
};
std::ostream &operator<<(std::ostream &os, const Work &w) {w.stream(os); return os;}
Work work;
#include "gubg/log/end.hpp"

enum class ReturnCode {MSS_DEFAULT_CODES,};

#define GUBG_MODULE_ "test_Factory"
#include "gubg/log/begin.hpp"
class Factory: public gubg::msgpack::Factory_crtp<Factory, std::string, 15>
{
    public:
		enum {work_rid = 123};
		void msgpack_beginOfFrame() {S();}
		void msgpack_endOfFrame() {S();}
        void msgpack_createObject(gubg::msgpack::Wrapper<std::string> &obj, RoleId rid)
        {
            SS(rid);
            switch (rid)
            {
                case work_rid: obj = wrap(work); break;
            }
        }
        void msgpack_createdObject(RoleId rid)
        {
            SS(rid);
            switch (rid)
            {
                case work_rid: L(STREAM(work)); break;
            }
        }
        void msgpack_set(RoleId rid, gubg::msgpack::Nil_tag) {S();L(rid << " nil");}
        void msgpack_set(RoleId rid, const std::string &str) {S();L(STREAM(rid, str));}
        void msgpack_set(RoleId rid, long l) {S();L(STREAM(rid, l));}
        void msgpack_set(RoleId rid, bool b) {S();L(STREAM(rid, b));}
};
#include "gubg/log/end.hpp"

namespace data
{
    using namespace helper;
    auto msg_0 = str_({0x00});
    auto msg_nil = str_({0xc0});
    auto msg_ut = str_({0x82, 0x7b, 0x84, 0x00, 0x12, 0x01, 0xa3, 0x61, 0x62, 0x63, 0x02, 0xc0, 0x03, 0xc2,
			                  0x7b, 0x84, 0x00, 0x34, 0x01, 0xa3, 0x41, 0x42, 0x43, 0x02, 0xc0, 0x03, 0xc3});
}

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    Factory f;
    //    f.process(data::msg_0);
    f.process(data::msg_nil);
    f.process(data::msg_ut);
    work.stream(std::cout);

    std::string buffer;
    gubg::msgpack::Serializer<std::string, 10> serializer;
    serializer.serialize(buffer);
    std::cout << gubg::testing::toHex(buffer) << std::endl;
    return 0;
}
#include "gubg/log/end.hpp"
