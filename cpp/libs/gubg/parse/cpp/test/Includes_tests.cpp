#include "gubg/Testing.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#define STRING() <string>
#include STRING()

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    struct Includes: gubg::parse::cpp::Includes_crtp<Includes>
    {
        template <typename String, typename Type>
            void includes_detected(const String &str, Type type)
            {
                S();L(STREAM(str, (int)type));
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
#include "gubg/log/end.hpp"
