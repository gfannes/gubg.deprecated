#ifndef HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_SyntaxTree_hpp_ALREADY_INCLUDED

#include "rinle/model/Types.hpp"
#include "gubg/tree/Node.hpp"

namespace rinle { namespace model { namespace syntax {

	struct Data
	{
		Range range;
	};
	typedef gubg::tree::Node<Data> Node;

	inline Node::Ptr createTree(Tokens &tokens)
	{
		auto ret = Node::create();
		ret->data.range = Range(tokens);
		return ret;
	}

} } }

#endif
