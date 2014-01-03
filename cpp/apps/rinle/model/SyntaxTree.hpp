#ifndef HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED

#include "rinle/model/Tokens.hpp"
#include "gubg/tree/Node.hpp"
#include "gubg/tree/dfs/Iterate.hpp"

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
            template <typename Path>
                bool open(Node::Ptr n, const Path &p)
                {
                    std::cout << "[" << p.size();
                    for (auto r = n->data.range; !r.empty(); r.popFront())
                    {
                        std::cout << r.front().token.toString();
                    }
                    std::cout << "]";
                    return true;
                }
            template <typename Path>
                void close(Node::Ptr n, const Path &p)
                {
                }
        private:
    };

    class Creater
    {
        public:
            Node::Ptr root;

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
                    gubg::tree::dfs::iterate_ptr(root, printer);
                }

                MSS_END();
            }

        private:
            ReturnCode createRoot_()
            {
                MSS_BEGIN(ReturnCode);
                root = Node::create();
                root->data.range = Range(tokens_);
                root->data.type = Root;
                MSS_END();
            }
            ReturnCode groupTokenWhitespace_()
            {
                MSS_BEGIN(ReturnCode);

                Tokens::iterator it = tokens_.begin();
                for (; it != tokens_.end() && it->token.isWhitespace(); ++it)
                {
                }
                auto n = Node::create(root);
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
                        n->data.range.end(std::next(it));
                        continue;
                    }
                    else if (isSymbol(it->token, '(') || isSymbol(it->token, '{'))
                    {
                        L("Opening an new range");
                        auto nn = Node::create(n->parent());
                        nn->data.range = Range(it, it);
                        nn->data.type = Scope;
                        n = Node::create(nn);
                        n->data.range = Range(it, std::next(it));
                    }
                    else if (isSymbol(it->token, ')') || isSymbol(it->token, '}'))
                    {
                        L("Closing a range");
                        n = Node::create(n->parent());
                        n->data.range = Range(it, std::next(it));
                        for (; it != tokens_.end(); ++it)
                        {
                            if (!it->token.isWhitespace())
                                break;
                            n->data.range.end(std::next(it));
                        }
                        n = n->parent();
                        if (n)
                            n->data.range.end(std::next(it));
                    }
                    else
                    {
                        L("Other: " << it->token.toString());
                        n = Node::create(n->parent());
                        n->data.range = Range(it, std::next(it));
                    }
                }

                root->childs.erase(root->childs.begin());

                MSS_END();
            }
            ReturnCode collateMacros_()
            {
                MSS_BEGIN(ReturnCode);
                decltype(root->childs) orig; orig.swap(root->childs);
                using namespace gubg::parse::cpp::pp;
                for (auto it = orig.begin(); it != orig.end(); ++it)
                {
                    S();L((*it)->data.range.front().token.toString());
                    if ((*it)->data.range.front().token.type != Token::MacroHash)
                    {
                        L("This is not the start of a macro");
                        addChild(root, *it);
                        continue;
                    }
                    L("This is the start of a macro");
                    auto n = Node::create(root);
                    n->data.range.begin((*it)->data.range.begin());
                    n->data.type = Macro;
                    for (; it != orig.end(); ++it)
                    {
                        addChild(n, *it);
                        n->data.range.end((*it)->data.range.end());
                        if ((*it)->data.range.contains([](const OrderedToken &ot){return isLineFeed(ot.token);}))
                            break;
                    }
                    if (it == orig.end())
                        break;
                }
                MSS_END();
            }
            struct CollateStatements
            {
                template <typename Path>
                    bool open(Node::Ptr n, const Path &p)
                    {
                        S();L("Opening level " << p.size() << ", type: " << (int)n->data.type);
                        switch (n->data.type)
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
                                    decltype(n->childs) orig; orig.swap(n->childs);
                                    using namespace gubg::parse::cpp::pp;
                                    for (auto it = orig.begin(); it != orig.end(); ++it)
                                    {
                                        S();L((*it)->data.range.front().token.toString());
                                        if ((*it)->data.type != Unknown || isSymbol((*it)->data.range.front().token))
                                        {
                                            L("This is not the start of a statement");
                                            addChild(n, *it);
                                            continue;
                                        }
                                        L("This is the start of a statement");
                                        auto nn = Node::create(n);
                                        nn->data.range.begin((*it)->data.range.begin());
                                        nn->data.type = Statement;
                                        for (; it != orig.end(); ++it)
                                        {
                                            if ((*it)->data.type != Unknown && (*it)->data.type != Scope)
                                                break;
                                            addChild(nn, *it);
                                            nn->data.range.end((*it)->data.range.end());
                                            if (isSymbol((*it)->data.range.front().token, ';'))
                                                break;
                                        }
                                        if (it == orig.end())
                                            break;
                                    }
                                    return true;
                                }
                                break;
                        }
                        return false;
                    }
                template <typename Path>
                    void close(Node::Ptr n, const Path &p)
                    {
                        S();L("Closing level " << p.size() << ", type: " << (int)n->data.type);
                    }
            };
            ReturnCode collateStatements_()
            {
                MSS_BEGIN(ReturnCode);
                CollateStatements collateStatements;
                gubg::tree::dfs::iterate_ptr(root, collateStatements);
                MSS_END();
            }

            Tokens &tokens_;
            Node::Ptr n_;
    };

} } }
#include "gubg/log/end.hpp"

#endif
