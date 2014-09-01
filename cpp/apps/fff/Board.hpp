#ifndef HEADER_fff_Board_hpp_ALREADY_INCLUDED
#define HEADER_fff_Board_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include "fff/Tag.hpp"
#include "fff/Value.hpp"
#include "fff/Tool.hpp"
#include <vector>
#include <map>
#include <set>
#include <thread>

namespace fff { 

	typedef std::pair<Tag, Value> TagValue;
	typedef std::vector<TagValue> TagValues;

    class Board
    {
        public:
			ReturnCode add(Tag, Value);

			TagValues getFrom(size_t ix) const;

			ReturnCode expand(ToolChain);

			size_t size() const;

        private:
			TagValues tagValues_;

			typedef std::set<Tag> Tags;
			typedef std::map<Value, Tags> TagsPerValue;
			TagsPerValue tagsPerValue_;

			typedef std::mutex Mutex;
			typedef std::lock_guard<Mutex> LockGuard;
			mutable Mutex mutex_;
    };

} 

#endif
