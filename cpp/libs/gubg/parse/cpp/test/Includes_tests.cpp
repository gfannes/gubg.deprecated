#include "gubg/testing/Testing.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#include "gubg/l.hpp"
#define STRING() <string>
#include STRING()

namespace 
{
    struct Includes: gubg::parse::cpp::Includes_crtp<Includes>
    {
        template <typename String, typename Type>
            void includes_detected(const String &str, Type type)
            {
                L(STREAM(str, (int)type));
            }
    };
}

int main()
{
    TEST_TAG(main);
    Includes includes;
    includes.process(gubg::file::File(__FILE__));
    return 0;
}
