#ifndef HEADER_fff_Board_hpp_ALREADY_INCLUDED
#define HEADER_fff_Board_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include "fff/Types.hpp"
#include "fff/Agent.hpp"
#include <vector>
#include <map>
#include <set>
#include <thread>

namespace fff { 

    class Board
    {
        public:
			ReturnCode add(Tag, Value);
			ReturnCode add(Tag, Value, TagValue);

			TagValues getFrom(size_t ix) const;
			Dependencies getDependencies(const TagValue &) const;

			Hash hash(const Dependencies &) const;

			ReturnCode addAgent(Agent_itf::Ptr);
			ReturnCode expand();

        private:
			Agents agents_;

			TagValues tagValues_;

			typedef std::set<Tag> Tags;
			typedef std::map<Value, Tags> TagsPerValue;
			TagsPerValue tagsPerValue_;

			typedef std::map<TagValue, Dependencies> DependenciesPerTV;
			DependenciesPerTV dependenciesPerTV_;

			typedef std::map<TagValue, Hash> HashPerTV;
			mutable HashPerTV hashPerTV_;

			typedef std::mutex Mutex;
			typedef std::lock_guard<Mutex> LockGuard;
			mutable Mutex mutex_;
    };

} 

#endif
