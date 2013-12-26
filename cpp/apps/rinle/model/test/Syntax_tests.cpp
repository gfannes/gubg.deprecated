#include "rinle/model/SyntaxTree.hpp"
#include "rinle/model/Tokens.hpp"
using namespace rinle::model;

int main()
{
    Tokens tokens;
    tokenize(tokens, "int main(){}");
    auto root = syntax::createTree(tokens);
    return 0;
}
