#ifndef HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED

#include "rinle/model/Tokens.hpp"
#include "gubg/tree/Node.hpp"
#include "gubg/tree/dfs/Iterate.hpp"

#define GUBG_MODULE_ "Syntax"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model { namespace syntax {

	struct Data
	{
		Range range;
	};
	typedef gubg::tree::Node<Data> Node;

    class Printer
    {
        public:
            template <typename Path>
            bool open(Node &n, const Path &p)
            {
                std::cout << "[" << p.size();
                for (auto r = n.data.range; !r.empty(); r.popFront())
                {
                    std::cout << r.front().token.toString();
                }
                std::cout << "]";
                return true;
            }
            template <typename Path>
            void close(Node &n, const Path &p)
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

				{
					Printer printer;
					gubg::tree::dfs::iterate_ptr(*root, printer);
				}

				MSS_END();
			}

		private:
			ReturnCode createRoot_()
			{
				MSS_BEGIN(ReturnCode);
				root = Node::create();
				root->data.range = Range(tokens_);
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

			Tokens &tokens_;
			Node::Ptr n_;
	};

} } }
#include "gubg/log/end.hpp"

#endif
