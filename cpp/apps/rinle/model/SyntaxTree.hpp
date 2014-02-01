#ifndef HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED

#include "rinle/model/Tokens.hpp"
#include "gubg/tree/Node.hpp"

#define GUBG_MODULE_ "Syntax"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model { namespace syntax {

    enum Type {Unknown, Root, Scope, Macro, Statement};
    struct Data
    {
        Type type;
        Range range;
        Data(): type(Unknown){}
    };
    typedef gubg::tree::Node<Data> Node;

    class Printer
    {
        public:
            bool open(Node n)
            {
                assert(n);
                std::cout << "[";
                for (auto r = n.data().range; !r.empty(); r.popFront())
                {
                    std::cout << r.front().token.toString();
                }
                std::cout << "]";
                return true;
            }
            void close(Node n)
            {
            }
    };

    class Creater
    {
        public:
            Node root;

            Creater(Tokens &tokens): tokens_(tokens) {}

            ReturnCode operator()()
            {
                MSS_BEGIN(ReturnCode);

                MSS(createRoot_());

                MSS(groupTokenWhitespace_());

                MSS(collateMacros_());

                MSS(collateStatements_());

                {
                    Printer printer;
                    gubg::tree::iterate_dfs(root, printer);
                }

                MSS_END();
            }

        private:
            ReturnCode createRoot_()
            {
                MSS_BEGIN(ReturnCode);
                root = Node::create();
                root.data().range = Range(tokens_);
                root.data().type = Root;
                MSS_END();
            }
            ReturnCode groupTokenWhitespace_()
            {
                MSS_BEGIN(ReturnCode);

                Tokens::iterator it = tokens_.begin();
                for (; it != tokens_.end() && it->token.isWhitespace(); ++it)
                {
                }
                auto n = root.pushChild(Node::create());
                for (; it != tokens_.end(); ++it)
                {
                    S();
                    if (!n)
                    {
                        L("n is null");
                    }
                    else if (it->token.isWhitespace())
                    {
                        L("Whitespace");
                        n.data().range.end(std::next(it));
                        continue;
                    }
                    else if (isSymbol(it->token, '(') || isSymbol(it->token, '{'))
                    {
                        L("Opening an new range");
                        auto nn = n.parent().pushChild(Node::create());
                        nn.data().range = Range(it, it);
                        nn.data().type = Scope;
                        n = nn.pushChild(Node::create());
                        n.data().range = Range(it, std::next(it));
                    }
                    else if (isSymbol(it->token, ')') || isSymbol(it->token, '}'))
                    {
                        L("Closing a range");
                        n = n.parent().pushChild(Node::create());
                        n.data().range = Range(it, std::next(it));
                        for (; it != tokens_.end(); ++it)
                        {
                            if (!it->token.isWhitespace())
                                break;
                            n.data().range.end(std::next(it));
                        }
                        n = n.parent();
                        if (n)
                            n.data().range.end(std::next(it));
                    }
                    else
                    {
                        L("Other: " << it->token.toString());
                        n = n.parent().pushChild(Node::create());
                        n.data().range = Range(it, std::next(it));
                    }
                }

                root.shiftChild();

                MSS_END();
            }
            ReturnCode collateMacros_()
            {
                MSS_BEGIN(ReturnCode);
                auto orig = root.swapChilds(Node::create());
                using namespace gubg::parse::cpp::pp;
                for (auto ch = orig.firstChild(); ch; ch = ch.nextSibling())
                {
                    S();L(ch.data().range.front().token.toString());
                    if (ch.data().range.front().token.type != Token::MacroHash)
                    {
                        L("This is not the start of a macro");
                        root.pushChild(ch);
                        continue;
                    }
                    L("This is the start of a macro");
                    auto n = root.pushChild(Node::create());
                    n.data().range.begin(ch.data().range.begin());
                    n.data().type = Macro;
                    for (; ch; ch = ch.nextSibling())
                    {
                        n.pushChild(ch);
                        n.data().range.end(ch.data().range.end());
                        if (ch.data().range.contains([](const OrderedToken &ot){return isLineFeed(ot.token);}))
                            break;
                    }
                    if (!ch)
                        break;
                }
                MSS_END();
            }
            struct CollateStatements
            {
                bool open(Node n)
                {
                    assert(n);
                    S();L("Opening type: " << (int)n.data().type);
                    switch (n.data().type)
                    {
                        case Macro:
                            return false;
                            break;

                        case Statement:
                            return true;
                            break;

                        case Root:
                        case Scope:
                            {
                                auto orig = n.swapChilds(Node::create());
                                using namespace gubg::parse::cpp::pp;
                                for (auto ch = orig.firstChild(); ch; ch = ch.nextSibling())
                                {
                                    S();L(ch.data().range.front().token.toString());
                                    if (ch.data().type != Unknown || isSymbol(ch.data().range.front().token))
                                    {
                                        L("This is not the start of a statement");
                                        n.pushChild(ch);
                                        continue;
                                    }
                                    L("This is the start of a statement");
                                    auto nn = n.pushChild(Node::create());
                                    nn.data().range.begin(ch.data().range.begin());
                                    nn.data().type = Statement;
                                    for (; ch; ch = ch.nextSibling())
                                    {
                                        if (ch.data().type != Unknown && ch.data().type != Scope)
                                            break;
                                        nn.pushChild(ch);
                                        nn.data().range.end(ch.data().range.end());
                                        if (isSymbol(ch.data().range.front().token, ';'))
                                            break;
                                    }
                                    if (!ch)
                                        break;
                                }
                                return true;
                            }
                            break;
                    }
                    return false;
                }
                void close(Node n)
                {
                    assert(n);
                    S();L("Closing type: " << (int)n.data().type);
                }
            };
            ReturnCode collateStatements_()
            {
                MSS_BEGIN(ReturnCode);
                CollateStatements collateStatements;
                gubg::tree::iterate_dfs(root, collateStatements);
                MSS_END();
            }

            Tokens &tokens_;
            Node n_;
    };

} } }
#include "gubg/log/end.hpp"

#endif
