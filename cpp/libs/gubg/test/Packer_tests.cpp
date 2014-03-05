#include "gubg/Packer.hpp"
#include "gubg/Testing.hpp"
#include <string>

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace  { 
    class SizeProtocol
    {
        public:
            template <typename String>
                void pack(String &header, String &body)
                {
                    S();
                    header.resize(1);
                    header[0] = body.size();
                    L(STREAM(header, body));
                }
            template <typename String, typename Range>
                bool append_header(String &header, Range &range)
                {
                    while (!range.empty())
                    {
                        header.push_back(range.front());
                        range.pop_front();
                        if (header.size() == 1)
                        {
                            nr_ = header[0];
                            return true;
                        }
                    }
                    return false;
                }
            template <typename String, typename Range>
                bool append_body(String &body, Range &range)
                {
                    while (!range.empty())
                    {
                        body.push_back(range.front());
                        range.pop_front();
                        if (body.size() == nr_)
                            return true;
                    }
                    return false;
                }

        private:
            size_t nr_;
    };
} 

int main()
{
    TEST_TAG(main);
    typedef gubg::Packer<std::string, std::string,SizeProtocol> Packer;
    Packer packer;
    TEST_EQ(Packer::Idle, packer.state());
    {
        auto sdu = packer.sdu_out();
        TEST_EQ(Packer::Sending, packer.state());
        sdu.ref().append("Hello World");
    }
    TEST_EQ(Packer::Sending, packer.state());
    auto pdu = packer.pdu_out();
    for (; !pdu.empty(); pdu.pop_front())
        L("0x" << std::hex << (int)pdu.front());
    TEST_EQ(Packer::Sending, packer.state());

    packer.clear();
    TEST_EQ(Packer::Idle, packer.state());
    const std::string buffer("\x0bHello World");
    auto range = gubg::packer::createRange(buffer);

    TEST_TRUE(packer.append(range));
    TEST_EQ(Packer::Receiving, packer.state());
    TEST_TRUE(packer.receivedFrame());
    TEST_EQ(packer.body(), "Hello World");
    packer.clear();
    TEST_FALSE(packer.receivedFrame());
    TEST_EQ(Packer::Idle, packer.state());
    range = gubg::packer::createRange(std::string("\x0bHello "));
    TEST_FALSE(packer.append(range));
    TEST_EQ(Packer::Receiving, packer.state());
    TEST_FALSE(packer.receivedFrame());
    range = gubg::packer::createRange(std::string("World"));
    TEST_TRUE(packer.append(range));
    TEST_TRUE(packer.receivedFrame());
    TEST_EQ(Packer::Receiving, packer.state());
    TEST_EQ(packer.body(), "Hello World");
    TEST_EQ(Packer::Receiving, packer.state());
    return 0;
}
#include "gubg/log/end.hpp"
