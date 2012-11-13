#define GUBG_LOG
#include "gubg/testing/Testing.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/l.hpp"
#include <vector>
using namespace gubg::parse::cpp;
using namespace gubg::parse::cpp::pp;
using namespace std;

int
m\
a\
i\
n\

()
{
    TEST_TAG(Lexer);
    Lexer<vector<Token>> lexer;
    Range range;
    TEST_OK(gubg::file::read(range, gubg::file::File(__FILE__)));
    TEST_OK(lexer.tokenize(range));
    L(STREAM(lexer.tokens().size()));
    for (auto token: lexer.tokens())
        cout << token;
    return 0;
}
