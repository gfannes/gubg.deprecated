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
					range = locus_->data.range;
					return true;
				}
				virtual bool move(Range &range, Direction dir)
				{
                    switch (dir)
                    {
                        case Direction::Forward:
                            {
                                auto ch = nextSibbling(locus_);
                                if (ch)
                                    locus_ = ch;
                            }
                            break;
                        case Direction::Backward:
                            {
                                auto ch = prevSibbling(locus_);
                                if (ch)
                                    locus_ = ch;
                            }
                            break;
                        case Direction::In:
                            if (!locus_->childs.empty())
                            {
                                switch (locus_->data.type)
                                {
                                    case syntax::Scope:
                                        assert(locus_->childs.size() >= 2);
                                        locus_ = locus_->childs[1];
                                        break;
                                    default:
                                        locus_ = locus_->childs.front();
                                        break;
                                }
                            }
                            break;
                        case Direction::Out:
                            {
                                auto p = locus_->parent();
                                if (p)
                                    locus_ = p;
                            }
                            break;
                        default:
                            return false;
                            break;
                    }
                    range = locus_->data.range;
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
				syntax::Node::Ptr root_;
				syntax::Node::Ptr locus_;
		};
	}
}
#include "gubg/log/end.hpp"

#endif
