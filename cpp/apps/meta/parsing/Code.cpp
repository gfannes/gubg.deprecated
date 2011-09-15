#include "parsing/Code.hpp"
using namespace meta;

CodeRange meta::reduce(CodeRange &range, unsigned int nr) throw(meta::OutOfRange)
{
    if (range.size() < nr)
        gubg::Exception::raise(OutOfRange());
    auto start = range.begin();
    range.advance_begin(nr);
    return CodeRange(start, range.begin());
}

Code meta::toCode(const CodeRange &cr)
{
    return Code(cr.begin(), cr.end());
}

#ifdef UnitTest
#include "testing/Testing.hpp"
using namespace std;
int main()
{
    TEST_TAG(Code);
    Code code("bla bli blo");
    CodeRange cr(code);
    auto front = reduce(cr, 3);
    TEST_EQ("bla", toCode(front));
    reduce(cr, 1);
    front = reduce(cr, 7);
    TEST_EQ("bli blo", toCode(front));
    TEST_THROW(meta::OutOfRange, reduce(cr, 1));
    return 0;
}
#endif
