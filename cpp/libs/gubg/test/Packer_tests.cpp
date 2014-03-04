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
    return 0;
}
#include "gubg/log/end.hpp"
