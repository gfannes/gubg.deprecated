#define GUBG_LOG
//#include "blabla.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/l.hpp"
#include <vector>
using namespace gubg::parse::cpp;
using namespace gubg::parse::cpp::pp;
using namespace std;

#define CONCAT(a,b) a ## b
#define STRINGIFY(a) #a

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
    int strAng3_identifier;
    int hex = 0x1234;
    return 0;
}
