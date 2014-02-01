#ifndef HEADER_rinle_model_ScopeNavigator_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_ScopeNavigator_hpp_ALREADY_INCLUDED

#include "rinle/model/Navigator.hpp"
#include "rinle/model/SyntaxTree.hpp"

#define GUBG_MODULE_ "ScopeNavigator"
#include "gubg/log/begin.hpp"
namespace rinle
{
    namespace model
    {
        class ScopeNavigator: public Navigator
        {
            public:
                ScopeNavigator(Tokens &tokens): Navigator(tokens) {}
                virtual bool set(Range &range) const
                {
                    S();
                    if (!locus_)
                        return false;
                    range = locus_.data().range;
                    return true;
                }
                virtual bool move(Range &range, Direction dir)
                {
                    switch (dir)
                    {
                        case Direction::Forward:
                            {
                                auto ch = locus_.nextSibling();
                                if (ch)
                                    locus_ = ch;
                            }
                            break;
                        case Direction::Backward:
                            {
                                auto ch = locus_.prevSibling();
                                if (ch)
                                    locus_ = ch;
                            }
                            break;
                        case Direction::In:
                            if (auto ch = locus_.firstChild())
                            {
                                switch (locus_.data().type)
                                {
                                    case syntax::Scope:
                                        assert(ch.nextSibling());
                                        locus_ = ch.nextSibling();
                                        break;
                                    default:
                                        locus_ = ch;
                                        break;
                                }
                            }
                            break;
                        case Direction::Out:
                            {
                                auto p = locus_.parent();
                                if (p)
                                    locus_ = p;
                            }
                            break;
                        default:
                            return false;
                            break;
                    }
                    range = locus_.data().range;
                    return true;
                }

                void refresh()
                {
                    recompute_();
                    locus_ = root_;
                }

            private:
                void recompute_()
                {
                    S();
                    syntax::Creater creater(tokens_);
                    if (!MSS_IS_OK(creater()))
                    {
                        L("Could not create the syntax tree");
                        return;
                    }
                    root_ = creater.root;
                }
                syntax::Node root_;
                syntax::Node locus_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
