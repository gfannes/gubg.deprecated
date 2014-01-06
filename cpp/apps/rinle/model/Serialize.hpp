#ifndef HEADER_rinle_model_Serialize_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Serialize_hpp_ALREADY_INCLUDED

#include "rinle/ReturnCode.hpp"
#include "rinle/model/Tree.hpp"
#include <string>

namespace rinle { namespace model {

	class Serialize_itf
	{
		public:
			virtual ReturnCode load(Node &, std::string src) = 0;
			virtual ReturnCode save(std::string &dst, const Node &) = 0;
	};

    class SerializeCPP: public Serialize_itf
    {
        public:
			virtual ReturnCode load(Node &, const std::string &src);
			virtual ReturnCode save(std::string &dst, const Node &);
    };

} }

#endif
