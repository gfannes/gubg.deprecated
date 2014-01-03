#ifndef HEADER_gubg_tree_dfs_Iterate_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_dfs_Iterate_hpp_ALREADY_INCLUDED

namespace gubg
{
    namespace tree
    {
        namespace dfs
        {
            namespace priv
            {
                namespace ref
                {
                    template <typename Node, typename Ftor, typename Path>
                        void iterate(Node &n, const Ftor &ftor, Path &path)
                        {
                            if (ftor.open(n, path))
                            {
                                path.push_back(&n);
                                for (auto &ch: n.childs)
                                    iterate(ch, ftor, path);
                                path.pop_back();
                                ftor.close(n, path);
                            }
                        }
                    template <typename Node, typename Ftor, typename Path>
                        void iterate(Node &n, Ftor &ftor, Path &path)
                        {
                            if (ftor.open(n, path))
                            {
                                path.push_back(&n);
                                for (auto &ch: n.childs)
                                    iterate(ch, ftor, path);
                                path.pop_back();
                                ftor.close(n, path);
                            }
                        }
                }
                namespace ptr
                {
                    template <typename NodePtr, typename Ftor, typename Path>
                        void iterate(NodePtr n, const Ftor &ftor, Path &path)
                        {
                            if (!n)
                                return;
                            if (ftor.open(n, path))
                            {
                                path.push_back(&*n);
                                for (auto ch: n->childs)
                                    iterate(ch, ftor, path);
                                path.pop_back();
                                ftor.close(n, path);
                            }
                        }
                    template <typename NodePtr, typename Ftor, typename Path>
                        void iterate(NodePtr n, Ftor &ftor, Path &path)
                        {
                            if (!n)
                                return;
                            if (ftor.open(n, path))
                            {
                                path.push_back(&*n);
                                for (auto ch: n->childs)
                                    iterate(ch, ftor, path);
                                path.pop_back();
                                ftor.close(n, path);
                            }
                        }
                }
            }

            //ftor.open() should indicate if iteration should recurse or not
            //close is only called if recursion should happen
            template <typename Node, typename Ftor>
                void iterate(Node &n, const Ftor &ftor)
                {
                    typedef std::vector<Node*> Path;
                    Path path;
                    priv::ref::iterate(n, ftor, path);
                }
            template <typename Node, typename Ftor>
                void iterate(Node &n, Ftor &ftor)
                {
                    typedef std::vector<Node*> Path;
                    Path path;
                    priv::ref::iterate(n, ftor, path);
                }
            //Use this version if a node keeps track of its childs using pointers (or something that looks like a pointer)
            template <typename NodePtr, typename Ftor>
                void iterate_ptr(NodePtr n, const Ftor &ftor)
                {
                    typedef typename std::pointer_traits<NodePtr>::element_type ET;
                    typedef std::vector<ET*> Path;
                    Path path;
                    priv::ptr::iterate(n, ftor, path);
                }
            template <typename NodePtr, typename Ftor>
                void iterate_ptr(NodePtr n, Ftor &ftor)
                {
                    typedef typename std::pointer_traits<NodePtr>::element_type ET;
                    typedef std::vector<ET*> Path;
                    Path path;
                    priv::ptr::iterate(n, ftor, path);
                }
        }
    }
}

#endif
