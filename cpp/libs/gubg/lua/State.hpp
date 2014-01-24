#ifndef HEADER_gubg_lua_State_hpp_ALREADY_INCLUDED
#define HEADER_gubg_lua_State_hpp_ALREADY_INCLUDED

#include "gubg/lua/Codes.hpp"
#include <memory>
#include <string>

namespace gubg { namespace lua { 

	class State
	{
		public:
			static State create();

			ReturnCode execute(const std::string &code);

			ReturnCode get(long &, const std::string &name) const;

		private:
			struct Pimpl;
			std::shared_ptr<Pimpl> pimpl_;
	};

} } 

#endif
