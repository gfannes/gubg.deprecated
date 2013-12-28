#include "rinle/model/SyntaxTree.hpp"
#include "rinle/model/Tokens.hpp"
using namespace rinle::model;

int main()
{
    Tokens tokens;
    tokenize(tokens, "int main(){int i;}");
    //tokenize(tokens, "{int i;}");
    auto root = syntax::createTree(tokens);
    syntax::Printer printer;
    gubg::tree::dfs::iterate_ptr(*root, printer);
    return 0;
}
