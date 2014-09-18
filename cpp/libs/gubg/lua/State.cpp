#include "gubg/lua/State.hpp"
#include "lua.hpp"
#include <mutex>
using namespace std;

#define GUBG_MODULE_ "State"
#include "gubg/log/begin.hpp"
namespace gubg { namespace lua { 

	struct State::Pimpl
	{
		lua_State *s;
		mutex mutex_;

		Pimpl(): s(luaL_newstate())
		{
			luaL_openlibs(s);
		}
		~Pimpl() {lua_close(s);}
	};

	State State::create()
	{
		State res; res.pimpl_.reset(new Pimpl);
		return res;
	}

	lua_State *State::raw()
	{
		if (!pimpl_)
			return 0;
		lock_guard<mutex> l_lock(pimpl_->mutex_);
		return pimpl_->s;
	}

#define CHECK_AND_LOCK(pimpl) MSS(pimpl); lock_guard<mutex> l_lock(pimpl->mutex_); auto s = pimpl_->s
	ReturnCode State::execute(const string &code)
	{
		string err;
		return execute(code, err);
	}
	ReturnCode State::execute(const string &code, string &err)
	{
		MSS_BEGIN(ReturnCode);
		CHECK_AND_LOCK(pimpl_);

		if (luaL_loadstring(s, code.c_str()) != 0)
		{
			err = lua_tostring(s, -1);
			MSS_L(CompileError);
		}
		err.clear();

		if (lua_pcall(s, 0, 0, 0) != 0)
		{
			err = lua_tostring(s, -1);
			MSS_L(RuntimeError);
		}
		err.clear();

		MSS_END();
	}
	ReturnCode State::get(long &v, const string &name) const
	{
		MSS_BEGIN(ReturnCode);
		CHECK_AND_LOCK(pimpl_);
		L(lua_gettop(s));
		lua_getglobal(s, name.c_str());
		L(lua_gettop(s));
		MSS((bool)lua_isnumber(s, -1));
		L(lua_gettop(s));
		v = lua_tointeger(s, -1);
		L(lua_gettop(s));
		lua_pop(s, 1);
		L(lua_gettop(s));
		MSS_END();
	}
	ReturnCode State::registerFunction(Function function, const string &name)
	{
		MSS_BEGIN(ReturnCode);
		CHECK_AND_LOCK(pimpl_);
		lua_pushcfunction(s, (lua_CFunction)function);
		lua_setglobal(s, name.c_str());
		MSS_END();
	}

} } 
#include "gubg/log/end.hpp"
