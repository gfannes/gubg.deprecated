#ifndef HEADER_rinle_model_Tree_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Tree_hpp_ALREADY_INCLUDED

#include "rinle/model/tree/Node.hpp"
#include <string>

namespace rinle
{
	namespace model
	{
		struct Data
		{
            std::string token;
		};
		typedef tree::Node<Data> Node;
	}
}

#endif
