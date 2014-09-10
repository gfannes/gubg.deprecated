#include "fff/Board.hpp"

#define GUBG_MODULE_ "Board"
#include "gubg/log/begin.hpp"
namespace fff { 

	size_t Board::size() const
	{
		LockGuard lg(mutex_);
		return tagValues_.size();
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
		LockGuard lg(mutex_);
		{
			const auto p = tagsPerValue_.find(value);
			MSS_Q(p == tagsPerValue_.end() || p->second.count(tag) == 0, TagValueAlreadyExists);
		}
		auto tv = TagValue(tag, value);
		tagValues_.push_back(tv);
		tagsPerValue_[value].insert(tag);
		dependenciesPerTV_[orig].insert(tv);
		MSS_END();
	}

	ReturnCode Board::setHash(TagValue tv, Hash hash)
	{
		MSS_BEGIN(ReturnCode);
		LockGuard lg(mutex_);
		{
			const auto p = hashPerTV_.find(tv);
			MSS_Q(p == hashPerTV_.end(), HashAlreadySet);
		}
		hashPerTV_[tv] = hash;
		MSS_END();
	}

	TagValues Board::getFrom(size_t ix) const
	{
		LockGuard lg(mutex_);
		if (ix >= tagValues_.size())
			return TagValues();
		return TagValues(tagValues_.begin()+ix, tagValues_.end());
	}

	ReturnCode Board::expand(ToolChain toolchain)
	{
        MSS_BEGIN(ReturnCode);
		auto it = toolchain.begin();
		while (it != toolchain.end())
		{
            auto &tool = **it;
            SS(tool.name());
			const auto s = size();

			tool.process(*this);

			if (size() != s)
				//The board was modified: restart
				it = toolchain.begin();
            else
                ++it;
		}
        MSS_END();
	}
} 
#include "gubg/log/end.hpp"
