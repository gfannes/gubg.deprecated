#ifndef HEADER_gubg_trace_Mgr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_Mgr_hpp_ALREADY_INCLUDED

#include "gubg/trace/Codes.hpp"
#include "gubg/trace/Tree.hpp"
#include "gubg/trace/DTO.hpp"
#include "gubg/Singleton.hpp"
#include "gubg/Platform.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/file/Descriptor.hpp"
#include "gubg/testing/Testing.hpp"
#include <thread>
#include <map>

#define GUBG_MODULE "trace::Mgr"
#include "gubg/log/begin.hpp"
namespace gubg { namespace trace { 
    class Mgr
    {
        public:
            static Mgr &instance()
            {
                thread_local Mgr *mgr = 0;
                if (!mgr)
                {
                    S();L("Creating local mgr pointer");
                    mgr = &Singleton<Mgr>::instance();
                }
                return *mgr;
            }
            void open(const Msg &msg)
            {
                S();L("Opening category " << msg.category() << " " << msg.str());
                getTree_().open(msg);
            }
            void close()
            {
                S();L("Closing");
                if (!getTree_().close())
                {
                    //The thread died, we move this tree to the dead trees
                    auto tree = cutTree_();
                    assert(tree);
                    LockGuard lg(mutex_);
                    deadTrees_.push_back(tree);
                }
            }

            void operator()()
            {
                S();
				file::Descriptor d;
                while (!quit_)
                {
					if (!d.valid())
					{
						d = file::Descriptor::connect("localhost", 1234);
                        if (d.valid())
                            std::cout << "Connected to ttt" << std::endl;
						continue;
					}

                    if (!MSS_IS_OK(work_(d)))
                        //Something went wrong, reset the network connection
                        d = file::Descriptor();

                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
#if 0
                TreePerTid lTreePerTid;
                {
                    LockGuard lg(mutex_);
                    lTreePerTid = treePerTid_;
                }
                for (auto p: lTreePerTid)
                {
                    std::cout << "Proportions: " << std::endl;
                    const auto stats = p.second->statistics();
                    for (auto prop: stats.proportions)
                        std::cout << prop << ", ";
                    std::cout << std::endl;
                }
#endif
            }

        private:
            ReturnCode work_(file::Descriptor d)
            {
                MSS_BEGIN(ReturnCode);
                TreePerTid lTreePerTid;
                {
                    LockGuard lg(mutex_);
                    lTreePerTid = treePerTid_;
                }
                for (auto p: lTreePerTid)
                {
                    auto scopes = p.second->getScopeOperations();
                    gubg::msgpack::Serializer<std::string, dto::TypeIds, 10> serializer;
                    for (auto &s: scopes)
                        serializer.serialize(s);
                    const auto str = serializer.buffer();
                    std::cout << "Sending " << str.size() << " bytes" << std::endl;
                    size_t s = 0;
                    while (s < str.size())
                    {
                        size_t tmp;
                        MSS(d.write(tmp, str.substr(s)));
                        s += tmp;
                    }
                }
                MSS_END();
            }
            friend class Singleton<Mgr>;
            Mgr(): thread_(std::ref(*this)) {}
            ~Mgr()
            {
                quit_ = true;
                thread_.join();
            }

            Tree &getTree_()
            {
                LockGuard lg(mutex_);
                const auto tid = threadId();
                Tree::Ptr &tree = treePerTid_[tid];
                if (!tree)
                    tree.reset(new Tree(tid));
                return *tree;
            }
            Tree::Ptr cutTree_()
            {
                Tree::Ptr tree;
                LockGuard lg(mutex_);
                auto it = treePerTid_.find(threadId());
                if (it != treePerTid_.end())
                {
                    tree = it->second;
                    treePerTid_.erase(it);
                }
                return tree;
            }

            typedef std::map<ThreadId, Tree::Ptr> TreePerTid;
            TreePerTid treePerTid_;
            typedef std::vector<Tree::Ptr> DeadTrees;
            DeadTrees deadTrees_;
            typedef std::mutex Mutex;
            typedef std::lock_guard<Mutex> LockGuard;
            Mutex mutex_;

            volatile bool quit_ = false;
            std::thread thread_;
    };
} } 
#include "gubg/log/end.hpp"

#endif
