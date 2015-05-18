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
            ReturnCode addItem(Tag, Value);
            ReturnCode addItem(Tag, Value, TagValue);

            bool isDirty() const {return isDirty_;}

            TagValues getFrom(size_t ix) const;
            Dependencies getDependencies(const TagValue &) const;
            RecursiveDependencies getRecursiveDependencies(const TagValue &, const Tags &excludes = Tags{}) const;

            ReturnCode addAgent(Agent_itf::Ptr);
            ReturnCode expand();

        private:
            //The agents looking and expanding the items on the board
            Agents agents_;

            //The items on the board
            TagValues tagValues_;

            //If this flag is set, expand will start with the first agent again.
            bool isDirty_ = true;

            //Faster checking if a tag-value already exists
            typedef std::set<Tag> Tags;
            typedef std::map<Value, Tags> TagsPerValue;
            TagsPerValue tagsPerValue_;

            typedef std::map<TagValue, Dependencies> DependenciesPerTV;
            DependenciesPerTV dependenciesPerTV_;

            typedef std::mutex Mutex;
            typedef std::lock_guard<Mutex> LockGuard;
            mutable Mutex mutex_;
    };

} 

#endif
