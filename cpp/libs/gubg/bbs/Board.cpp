#include "gubg/bbs/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/string_algo.hpp"
#include <cassert>

#define GUBG_MODULE_ "Board"
#include "gubg/log/begin.hpp"
namespace gubg { namespace bbs { 

    ReturnCode Board::addItem(Tag tag, Value value)
    {
        MSS_BEGIN(ReturnCode, STREAM(tag, value));
        LockGuard lg(mutex_);
        {
            const auto p = tagsPerValue_.find(value);
            MSS_Q(p == tagsPerValue_.end() || p->second.count(tag) == 0, TagValueAlreadyExists);
        }
        items_.push_back(Item(tag, value));
        tagsPerValue_[value].insert(tag);
        isDirty_ = true;
        MSS_END();
    }
    ReturnCode Board::addItem(const TagParts &parts, Value v)
    {
        return addItem(gubg::string_algo::join(parts, '.'), v);
    }
    ReturnCode Board::addItem(Tag tag, Value value, Item orig)
    {
        MSS_BEGIN(ReturnCode, STREAM(tag, value));
        auto tv = Item(tag, value);
        LockGuard lg(mutex_);
        dependenciesPerTV_[orig].insert(tv);
        {
            const auto p = tagsPerValue_.find(value);
            MSS_Q(p == tagsPerValue_.end() || p->second.count(tag) == 0, TagValueAlreadyExists);
        }
        items_.push_back(tv);
        tagsPerValue_[value].insert(tag);
        isDirty_ = true;
        MSS_END();
    }


    Items Board::getFrom(size_t ix) const
    {
        LockGuard lg(mutex_);
        if (ix >= items_.size())
            return Items{};
        return Items{items_.begin()+ix, items_.end()};
    }
    Dependencies Board::getDependencies(const Item &tv) const
    {
        Dependencies deps;
        LockGuard lg(mutex_);
        auto p = dependenciesPerTV_.find(tv);
        if (p != dependenciesPerTV_.end())
            deps = p->second;
        return deps;
    }
    RecursiveDependencies Board::getRecursiveDependencies(const Item &tv, const Tags &excludes) const
    {
        RecursiveDependencies rdeps;
        Dependencies new_ones = getDependencies(tv);
        rdeps[tv] = new_ones;
        while (true)
        {
            Dependencies stage;
            for (auto new_tv: new_ones)
            {
                if (rdeps.count(new_tv))
                    continue;
                if (excludes.count(new_tv.tag))
                    continue;
                auto tmp = getDependencies(new_tv);
                rdeps[new_tv] = tmp;
                stage.insert(tmp.begin(), tmp.end());
            }
            stage.swap(new_ones);
            if (stage.empty())
                //No more potential childs to check
                break;
        }
        return rdeps;
    }

    ReturnCode Board::addAgent(Agent_itf::Ptr agent)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)agent);
        L(agent->name());
        LockGuard lg(mutex_);
        for (auto t: agents_)
        {
            const auto rc = (t->name() != agent->name());
            MSS(rc, AgentAlreadyPresent);
        }
        agents_.push_back(agent);
        MSS_END();
    }
    ReturnCode Board::expand()
    {
        MSS_BEGIN(ReturnCode);
        isDirty_ = false;
        size_t ix = 0;
        while (true)
        {
            Agent_itf::Ptr agent_ptr;
            {
                LockGuard lg(mutex_);
                if (ix >= agents_.size())
                    //We are done, no more agents to run
                    break;
                agent_ptr = agents_[ix];
            }

            assert(agent_ptr);
            auto &agent = *agent_ptr;

            SS(agent.name());
            MSS(agent.process(*this));

            if (isDirty_)
            {
                //Restart
                ix = 0;
                isDirty_ = false;
            }
            else
                //Continue
                ++ix;
        }
        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
