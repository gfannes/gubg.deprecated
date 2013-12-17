#include "gubg/AString.hpp"
#include "gubg/Testing.hpp"

enum Flags
{
    A, B, C,
    Nr
};
enum class Flags_ec
{
    A, B, C,
    Nr
};

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(enum);
        gubg::AString<Flags> astr;
        TEST_FALSE(astr.flags[A]);
        TEST_FALSE(astr.flags[B]);
        TEST_FALSE(astr.flags[C]);
    }
    {
        TEST_TAG(enum_class);
        gubg::AString<Flags_ec> astr;
        TEST_FALSE(astr.flags[(int)Flags_ec::A]);
        TEST_FALSE(astr.flags[(int)Flags_ec::B]);
        TEST_FALSE(astr.flags[(int)Flags_ec::C]);
    }
    return 0;
}
#include "gubg/log/end.hpp"
