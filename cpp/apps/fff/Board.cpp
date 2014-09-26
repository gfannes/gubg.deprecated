#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include <cassert>

namespace  { 
	typedef std::map<lua_State*, fff::Board*> BoardPerLuaState;
	BoardPerLuaState boardPerLuaState__;
} 

#define GUBG_MODULE_ "Board"
#include "gubg/log/begin.hpp"
extern "C"
{
#include "lua.h"
	int fff_board_add(lua_State *s)
	{
		SS(lua_gettop(s));
		int nr_res = 0;
		if (lua_gettop(s) != 2)
		{
			lua_pushboolean(s, false); ++nr_res;
			return nr_res;
		}

		if (lua_type(s, -2) != LUA_TSTRING)
		{
			L("First arg should be a string");
			lua_pushboolean(s, false); ++nr_res;
			return nr_res;
		}
		auto tag = fff::Tag(gubg::string_algo::split<std::vector>(lua_tostring(s, -2), "."));

		if (lua_type(s, -1) != LUA_TTABLE)
		{
			L("Second arg should be a table");
			lua_pushboolean(s, false); ++nr_res;
			return nr_res;
		}
		std::unique_ptr<fff::Value> value;
		if (!value)
		{
			lua_pushstring(s, "file");
			lua_gettable(s, -2);
			if (!lua_isnil(s, -1))
				value.reset(new fff::Value(gubg::file::File(lua_tostring(s, -1))));
			lua_pop(s, 1);
		}
		if (!value)
		{
			lua_pushstring(s, "str");
			lua_gettable(s, -2);
			if (!lua_isnil(s, -1))
				value.reset(new fff::Value(lua_tostring(s, -1)));
			lua_pop(s, 1);
		}
		if (!value)
		{
			lua_pushstring(s, "integer");
			lua_gettable(s, -2);
			if (!lua_isnil(s, -1))
				value.reset(new fff::Value(long(lua_tonumber(s, -1))));
			lua_pop(s, 1);
		}
		if (!value)
		{
			L("Could not intepret the value");
			return nr_res;
		}

		auto it = boardPerLuaState__.find(s);
		if (it == boardPerLuaState__.end())
		{
			L("Could not find the board");
			return nr_res;
		}
		auto &board = *it->second;

		board.add(tag, *value);

		lua_pushboolean(s, true); ++nr_res;
		return nr_res;
	}
}
namespace fff { 

	const std::string lua_lib__ = "board = {add=fff_board_add};";

	Board::Board():
		luaState_(gubg::lua::State::create())
	{
		luaState_.registerFunction(fff_board_add, "fff_board_add");
		luaState_.execute(lua_lib__);
		boardPerLuaState__[luaState_.raw()] = this;
	}
	Board::~Board()
	{
		boardPerLuaState__.erase(luaState_.raw());
	}

	ReturnCode Board::add(Tag tag, Value value)
	{
		MSS_BEGIN(ReturnCode, STREAM(tag, value));
		LockGuard lg(mutex_);
		{
			const auto p = tagsPerValue_.find(value);
			MSS_Q(p == tagsPerValue_.end() || p->second.count(tag) == 0, TagValueAlreadyExists);
		}
		tagValues_.push_back(TagValue(tag, value));
		tagsPerValue_[value].insert(tag);
		MSS_END();
	}
	ReturnCode Board::add(Tag tag, Value value, TagValue orig)
	{
		MSS_BEGIN(ReturnCode, STREAM(tag, value));
		auto tv = TagValue(tag, value);
		LockGuard lg(mutex_);
		dependenciesPerTV_[orig].insert(tv);
		{
			const auto p = tagsPerValue_.find(value);
			MSS_Q(p == tagsPerValue_.end() || p->second.count(tag) == 0, TagValueAlreadyExists);
		}
		tagValues_.push_back(tv);
		tagsPerValue_[value].insert(tag);
		MSS_END();
	}


	TagValues Board::getFrom(size_t ix) const
	{
		LockGuard lg(mutex_);
		if (ix >= tagValues_.size())
			return TagValues();
		return TagValues(tagValues_.begin()+ix, tagValues_.end());
	}
	Dependencies Board::getDependencies(const TagValue &tv) const
	{
		Dependencies deps;
		LockGuard lg(mutex_);
		auto p = dependenciesPerTV_.find(tv);
		if (p != dependenciesPerTV_.end())
			deps = p->second;
		return deps;
	}
	Hash Board::hash(const Dependencies &deps) const
	{
		S();
		Hash hash;
		using namespace gubg::hash::md5;
		for (auto tv: deps)
		{
			LockGuard lg(mutex_);
			auto p = hashPerTV_.find(tv);
			if (p == hashPerTV_.end())
			{
				Stream md5;
				md5 << tv.first.to_str() << tv.second.to_str();
				switch (tv.second.type())
				{
					case Value::File:
						{
							std::string content; gubg::file::read(content, tv.second.file());
							md5 << content;
						}
						break;
				}
				hashPerTV_[tv] = md5.hash();
				p = hashPerTV_.find(tv);
			}
			assert(p != hashPerTV_.end());
			L(p->second.to_hex() << ": " << tv.second);
			hash ^= p->second;
		}
		L("Final hash: " << hash.to_hex());
		return hash;
	}

	ReturnCode Board::addTool(Tool_itf::Ptr tool)
	{
		MSS_BEGIN(ReturnCode);
		MSS((bool)tool);
		L(tool->name());
		LockGuard lg(mutex_);
		for (auto t: toolChain_)
			MSS(t->name() != tool->name(), ToolAlreadyPresent);
		toolChain_.push_back(tool);
		MSS_END();
	}
	ReturnCode Board::executeLua(const std::string &code, std::string &err)
	{
		MSS_BEGIN(ReturnCode);
		MSS(luaState_.execute(code, err));
		MSS_END();
	}
	ReturnCode Board::expand()
	{
        MSS_BEGIN(ReturnCode);
		size_t ix = 0;
		while (true)
		{
			size_t board_size, toolchain_size;
			Tool_itf::Ptr tool_ptr;
			{
				LockGuard lg(mutex_);
				board_size = tagValues_.size();
				toolchain_size = toolChain_.size();
				if (ix >= toolchain_size)
					//We are done, no more tools to run
					break;
				tool_ptr = toolChain_[ix];
			}

			assert(tool_ptr);
			auto &tool = *tool_ptr;

            SS(tool.name());
			MSS(tool.process(*this));

			bool boardWasModified = false;
			{
				LockGuard lg(mutex_);
				if (tagValues_.size() != board_size)
					boardWasModified = true;
				if (toolChain_.size() != toolchain_size)
					boardWasModified = true;
			}
			if (boardWasModified)
				//Restart
				ix = 0;
            else
				//Continue
                ++ix;
		}
        MSS_END();
	}
} 
#include "gubg/log/end.hpp"
