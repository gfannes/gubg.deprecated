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
					if (!root_)
						return false;
					range = root_->data.range;
					return true;
				}
				virtual bool move(Range &range, Direction dir)
				{
					return false;
				}

				void refresh()
				{
					recompute_();
				}

			private:
				void recompute_()
				{
					root_ = syntax::createTree(tokens_);
				}
				syntax::Node::Ptr root_;
		};
	}
}
#include "gubg/log/end.hpp"

#endif
