#ifndef HEADER_fff_Board_hpp_ALREADY_INCLUDED
#define HEADER_fff_Board_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include "fff/Tag.hpp"
#include "fff/Value.hpp"
#include "fff/Tool.hpp"
#include "gubg/hash/MD5.hpp"
#include <vector>
#include <map>
#include <set>
#include <thread>

namespace fff { 

	typedef std::pair<Tag, Value> TagValue;
	typedef std::vector<TagValue> TagValues;
	typedef gubg::hash::MD5::Hash Hash;

    class Board
    {
        public:
			ReturnCode add(Tag, Value);
			ReturnCode add(Tag, Value, TagValue);

			ReturnCode setHash(TagValue, Hash);

			TagValues getFrom(size_t ix) const;

			ReturnCode expand(ToolChain);

			size_t size() const;

        private:
			TagValues tagValues_;

			typedef std::set<Tag> Tags;
			typedef std::map<Value, Tags> TagsPerValue;
			TagsPerValue tagsPerValue_;

			typedef std::set<TagValue> Dependencies;
			typedef std::map<TagValue, Dependencies> DependenciesPerTV;
			DependenciesPerTV dependenciesPerTV_;

			typedef std::map<TagValue, Hash> HashPerTV;
			HashPerTV hashPerTV_;

			typedef std::mutex Mutex;
			typedef std::lock_guard<Mutex> LockGuard;
			mutable Mutex mutex_;
    };

} 

#endif
