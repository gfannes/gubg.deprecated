#ifndef HEADER_gubg_trace_Mgr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_Mgr_hpp_ALREADY_INCLUDED

#include "gubg/trace/Msg.hpp"
#include "gubg/tree/Node.hpp"
#include "gubg/Singleton.hpp"
#include <thread>
#include <memory>
#include <map>

#define GUBG_MODULE_ "trace::Mgr"
#include "gubg/log/begin.hpp"
namespace gubg { namespace trace { 
    class Tree
    {
        public:
            typedef std::shared_ptr<Tree> Ptr;
            Tree(std::thread::id tid): tid_(tid) {}

            void open(const Msg &msg)
            {
                if (!root_)
                {
                    root_ = Node::create();
                    back_ = root_;
                }
                else
                    back_ = back_.pushChild(Node::create());

                back_.data().str = msg.str();
                back_.data().category = msg.category();
            }
            //Returns false if the thread died
            bool close()
            {
                back_ = back_.parent();
                return back_;
            }

        private:
            struct Data
            {
                std::string str;
                int category = 0;
            };
            typedef gubg::tree::Node<Data> Node;

            const std::thread::id tid_;

            Node root_;
            Node back_;
            std::mutex mutex_;
    };
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
                while (!quit_)
                {
                }
            }

        private:
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
                const auto tid = std::this_thread::get_id();
                Tree::Ptr &tree = treePerTid_[tid];
                if (!tree)
                    tree.reset(new Tree(tid));
                return *tree;
            }
            Tree::Ptr cutTree_()
            {
                Tree::Ptr tree;
                LockGuard lg(mutex_);
                auto it = treePerTid_.find(std::this_thread::get_id());
                if (it != treePerTid_.end())
                {
                    tree = it->second;
                    treePerTid_.erase(it);
                }
                return tree;
            }

            typedef std::map<std::thread::id, Tree::Ptr> TreePerTid;
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
