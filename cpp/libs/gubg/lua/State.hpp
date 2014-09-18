#ifndef HEADER_gubg_lua_State_hpp_ALREADY_INCLUDED
#define HEADER_gubg_lua_State_hpp_ALREADY_INCLUDED

#include "gubg/lua/Codes.hpp"
#include <memory>
#include <string>

extern "C"
{
	struct lua_State;
}

namespace gubg { namespace lua { 

	extern "C"
	{
		typedef int (*Function) (lua_State *L);
	}

	class State
	{
		public:
			static State create();

			ReturnCode execute(const std::string &code);
			ReturnCode execute(const std::string &code, std::string &err);

			ReturnCode get(long &, const std::string &name) const;

			ReturnCode registerFunction(Function, const std::string &name);

			lua_State *raw();

		private:
			struct Pimpl;
			std::shared_ptr<Pimpl> pimpl_;
	};

} } 

#endif
