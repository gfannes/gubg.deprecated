#ifndef HEADER_gubg_tree_tree_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_tree_hpp_ALREADY_INCLUDED

#include <memory>
#include <vector>

namespace gubg
{
    template <typename Leaf, typename HubData>
        class Hub: HubData
    {
        public:
		Hub(){}
		Hub(Hub &&dead):
			childs_(std::move(dead.childs_)){}
		Hub &operator=(Hub &&dead)
		{
			childs_ = std::move(dead.childs_);
		}

            template <typename T>
                void push_back(T &&v)
                {
                    childs_.push_back(Node(std::move(v)));
                }

        private:
            class Node
	    {
		    public:
			    std::unique_ptr<Leaf> leaf;
			    std::unique_ptr<Hub> hub;

			    Node(Node &&dead):
				    leaf(std::move(dead.leaf)),
				    hub(std::move(dead.hub)){}
			    Node &operator=(Node &&dead)
			    {
				    leaf = std::move(dead.leaf);
				    hub = std::move(dead.hub);
			    }
			    Node(Leaf &&l):
				    leaf(new Leaf) { *leaf = std::move(l); }
			    Node(Hub &&h):
				    hub(new Hub) { *hub = std::move(h); }
		    private:
			    Node(const Node &);
			    Node &operator=(const Node &);
	    };
            typedef std::vector<Node> Nodes;
            Nodes childs_;

        public:
            struct Link
            {
                typedef Hub HubT;
                HubT *hub;
                typename Nodes::iterator child;
                Link(Hub *h, typename Nodes::iterator c):
                    hub(h), child(c){}
                bool operator==(const Link &rhs) const
                {
                    return hub == rhs.hub && child == rhs.child;
                }
            };
            struct ConstLink
            {
                typedef const Hub HubT;
                HubT *hub;
                typename Nodes::const_iterator child;
                ConstLink(const Hub *h, typename Nodes::const_iterator c):
                    hub(h), child(c){}
                bool operator==(const ConstLink &rhs) const
                {
                    return hub == rhs.hub && child == rhs.child;
                }
            };
            typedef std::vector<Hub*> Path;
            template <typename LinkT>
                class iterator_
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
                        bool isHub() const {return (bool)path_.back().child->hub;}

                        bool isEnd() const {return path_.size() == 1 && path_[0].hub->childs_.end() == path_[0].child;}

                        bool operator==(const iterator_ &rhs) const
                        {
                            return path_ == rhs.path_;
                        }
                        bool operator!=(const iterator_ &rhs) const
                        {
                            return !(*this == rhs);
                        }

                        //Will proceed DFS-style until the end is reached
                        iterator_ &operator++()
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

                        typename LinkT::HubT &parent() const {return *path_.back().hub;}

                    protected:
                        Leaf &leaf() const {return *path_.back().child->leaf;}
                        Hub &hub() const {return *path_.back().child->hub;}

                    private:
                        //Assumes we are _not_ at the end
                        void proceedOneStep_()
                        {
                            {
                                LinkT &link = path_.back();
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
                                        path_.push_back(LinkT(newHub, newHub->childs_.begin()));
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
                                LinkT &link = path_.back();
                                ++link.child;
                                if (link.hub->childs_.end() != link.child)
                                    //We can proceed sideways
                                    return;
                            }
                        }

                        friend class Hub;
                        typedef std::vector<LinkT> LinkPath;
                        LinkPath path_;
                };
            class iterator: public iterator_<Link>
        {
            public:
                Leaf &leaf() const {return iterator_<Link>::leaf();}
        };
            class const_iterator: public iterator_<ConstLink>
        {
            public:
                const Leaf &leaf() const {return iterator_<ConstLink>::leaf();}
        };
            iterator begin()
            {
                iterator res;
                res.path_.push_back(Link(this, childs_.begin()));
                if (!res.isLeaf())
                    ++res;
                return res;
            }
            const_iterator begin() const
            {
                const_iterator res;
                res.path_.push_back(ConstLink(this, childs_.begin()));
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
            const_iterator end() const
            {
                const_iterator res;
                res.path_.push_back(ConstLink(this, childs_.end()));
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
