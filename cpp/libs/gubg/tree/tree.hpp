#ifndef gubg_tree_tree_hpp
#define gubg_tree_tree_hpp

#include <memory>
#include <vector>

namespace gubg
{
    template <typename Leaf, typename HubData>
        class Hub: HubData
        {
            public:
                template <typename T>
                void push_back(T &&v)
                {
                    childs_.push_back(Node(std::move(v)));
                }

            private:
                struct Node
                {
                    std::unique_ptr<Leaf> leaf;
                    std::unique_ptr<Hub> hub;

                    Node(Leaf &&l):
                        leaf(new Leaf) { *leaf = std::move(l); }
                    Node(Hub &&h):
                        hub(new Hub) { *hub = std::move(h); }
                };
                typedef std::vector<Node> Nodes;
                Nodes childs_;

            public:
                struct Link
                {
                    Hub *hub;
                    typename Nodes::iterator child;
                    Link(Hub *h, typename Nodes::iterator c):
                        hub(h), child(c){}
                    bool operator==(const Link &rhs) const
                    {
                        return hub == rhs.hub && child == rhs.child;
                    }
                };
                typedef std::vector<Hub*> Path;
                class iterator
                {
                    public:
                        const Path path() const
                        {
                            Path path;
                            for (auto link = path_.begin(); link != path_.end(); ++link)
                                path.push_back(link->hub);
                            return path;
                        }
                        bool isLeaf() const {return (bool)path_.back().child->leaf;}
                        Leaf &leaf(){return *path_.back().child->leaf;}
                        bool isHub() const {return (bool)path_.back().child->hub;}
                        Hub &hub(){return *path_.back().child->hub;}

                        bool isEnd() const {return path_.size() == 1 && path_[0].hub->childs_.end() == path_[0].child;}

                        bool operator==(const iterator &rhs) const
                        {
                            return path_ == rhs.path_;
                        }
                        bool operator!=(const iterator &rhs) const
                        {
                            return !(*this == rhs);
                        }

                        //Will proceed DFS-style until the end is reached
                        iterator &operator++()
                        {
                            if (isEnd())
                                return *this;
                            proceedOneStep_();
                            while (!isEnd())
                            {
                                if (isLeaf())
                                    return *this;
                                proceedOneStep_();
                            }
                            return *this;
                        }
                    private:
                        //Assumes we are _not_ at the end
                        void proceedOneStep_()
                        {
                            {
                                Link &link = path_.back();
                                if (link.child->hub)
                                {
                                    Hub *newHub = link.child->hub.get();
                                    if (newHub->childs_.empty())
                                    {
                                        //This hub has no childs
                                    }
                                    else
                                    {
                                        //We can go deeper
                                        path_.push_back(Link(newHub, newHub->childs_.begin()));
                                        return;
                                    }
                                }
                                ++link.child;
                                if (link.hub->childs_.end() != link.child)
                                    //We can proceed sideways
                                    return;
                            }
                            //We have to get down again
                            while (path_.size() >= 2)
                            {
                                path_.pop_back();
                                Link &link = path_.back();
                                ++link.child;
                                if (link.hub->childs_.end() != link.child)
                                    //We can proceed sideways
                                    return;
                            }
                        }

                        friend class Hub;
                        typedef std::vector<Link> LinkPath;
                        LinkPath path_;
                };
                iterator begin()
                {
                    iterator res;
                    res.path_.push_back(Link(this, childs_.begin()));
                    if (!res.isLeaf())
                        ++res;
                    return res;
                }
                iterator end()
                {
                    iterator res;
                    res.path_.push_back(Link(this, childs_.end()));
                    return res;
                }
        };

    struct NoData { };

    template <typename LeafT, typename HubData = NoData>
        struct Tree
        {
            typedef LeafT Leaf;
            typedef gubg::Hub<Leaf, HubData> Hub;
            typedef typename Hub::Path Path;
        };
}

#endif
