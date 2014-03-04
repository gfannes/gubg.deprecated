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

        private:
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
    return 0;
}
#include "gubg/log/end.hpp"
