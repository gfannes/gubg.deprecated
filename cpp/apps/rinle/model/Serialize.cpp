#include "rinle/model/Serialize.hpp"
#include "gubg/SmartRange.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include <vector>
using namespace std;

#define GUBG_MODULE "Serialize"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model {

    ReturnCode load(Node &tree, string src)
    {
        MSS_BEGIN(ReturnCode);
        typedef gubg::SmartRange<std::string> Range;
        typedef std::vector<Token> Tokens;
        Range range(std::move(src));
        gubg::parse::cpp::pp::Lexer<Tokens> lexer;
        MSS(lexer.tokenize(range));
        tree = Node::create();
        for (auto it = lexer.tokens().begin(); it != lexer.tokens().end(); ++it)
        {
            auto n = Node::create(tree);
            n->data().token = it->toString();
        }
        MSS_END();
    }

    ReturnCode save(string &dst, const Node &tree)
    {
        MSS_BEGIN(ReturnCode);
        MSS_END();
    }

} }
#include "gubg/log/end.hpp"
