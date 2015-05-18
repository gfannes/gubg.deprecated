#ifndef HEADER_fff_Board_hpp_ALREADY_INCLUDED
#define HEADER_fff_Board_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Codes.hpp"
#include "gubg/bbs/Item.hpp"
#include "gubg/bbs/Agent.hpp"
#include "gubg/file/File.hpp"
#include <vector>
#include <map>
#include <set>
#include <thread>

namespace gubg { namespace bbs { 

    using Dependencies = std::set<Item>;
    using RecursiveDependencies = std::map<Item, Dependencies>;

    class Board
    {
        public:
            using TagParts = std::vector<std::string>;

            ReturnCode addItem(Tag, Value);
            ReturnCode addItem(Tag t, file::File f){return addItem(t, f.name());}
            ReturnCode addItem(const TagParts &, Value);
            ReturnCode addItem(Tag, Value, Item);
            ReturnCode addItem(Tag t, file::File f, Item p){return addItem(t, f.name(), p);}

            bool isDirty() const {return isDirty_;}

            Items getFrom(size_t ix) const;
            Dependencies getDependencies(const Item &) const;
            RecursiveDependencies getRecursiveDependencies(const Item &, const Tags &excludes = Tags{}) const;

            ReturnCode addAgent(Agent_itf::Ptr);
            ReturnCode expand();

        private:
            //The agents looking and expanding the items on the board
            Agents agents_;

            //The items on the board
            Items items_;

            //If this flag is set, expand will start with the first agent again.
            bool isDirty_ = true;

            //Faster checking if a tag-value already exists
            typedef std::set<Tag> Tags;
            typedef std::map<Value, Tags> TagsPerValue;
            TagsPerValue tagsPerValue_;

            typedef std::map<Item, Dependencies> DependenciesPerTV;
            DependenciesPerTV dependenciesPerTV_;

            typedef std::mutex Mutex;
            typedef std::lock_guard<Mutex> LockGuard;
            mutable Mutex mutex_;
    };

} } 

#endif
