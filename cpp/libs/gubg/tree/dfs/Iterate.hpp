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
                    template <typename Node, typename Ftor, typename Path>
                        void iterate(Node &n, const Ftor &ftor, Path &path)
                        {
                            if (ftor.open(n, path))
                            {
                                path.push_back(&n);
                                for (auto ch: n.childs)
                                    iterate(*ch, ftor, path);
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
                                for (auto ch: n.childs)
                                    iterate(*ch, ftor, path);
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
            template <typename Node, typename Ftor>
                void iterate_ptr(Node &n, const Ftor &ftor)
                {
                    typedef std::vector<Node*> Path;
                    Path path;
                    priv::ptr::iterate(n, ftor, path);
                }
            template <typename Node, typename Ftor>
                void iterate_ptr(Node &n, Ftor &ftor)
                {
                    typedef std::vector<Node*> Path;
                    Path path;
                    priv::ptr::iterate(n, ftor, path);
                }
        }
    }
}

#endif
