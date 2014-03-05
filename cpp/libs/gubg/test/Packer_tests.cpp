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
    {
        auto sdu = packer.sdu_out();
        sdu.ref().append("Hello World");
    }
    auto pdu = packer.pdu_out();
    for (; !pdu.empty(); pdu.pop_front())
        L("0x" << std::hex << (int)pdu.front());

    packer.clear();
    const std::string buffer("\x0bHello World");
    auto range = gubg::packer::createRange(buffer);

    TEST_TRUE(packer.append(range));
    TEST_EQ(packer.body(), "Hello World");
    packer.clear();
    range = gubg::packer::createRange(std::string("\x0bHello "));
    TEST_FALSE(packer.append(range));
    range = gubg::packer::createRange(std::string("World"));
    TEST_TRUE(packer.append(range));
    TEST_EQ(packer.body(), "Hello World");
    return 0;
}
#include "gubg/log/end.hpp"
