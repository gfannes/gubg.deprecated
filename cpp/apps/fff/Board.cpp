#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include <cassert>

#define GUBG_MODULE_ "Board"
#include "gubg/log/begin.hpp"

namespace fff { 

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
				md5 << tv.tag.to_str() << tv.tag.to_str();
				switch (tv.value.type())
				{
					case Value::File:
						{
							std::string content; gubg::file::read(content, tv.value.as_file());
							md5 << content;
						}
						break;
				}
				hashPerTV_[tv] = md5.hash();
				p = hashPerTV_.find(tv);
			}
			assert(p != hashPerTV_.end());
			L(p->second.to_hex() << ": " << tv.value);
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
