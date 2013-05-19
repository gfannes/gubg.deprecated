#ifndef aoeoaeu
#define aoeoaeu

namespace gubg
{
    namespace tree
    {
        namespace dfs
        {
            namespace priv
            {
                template <typename Node, typename Ftor, typename Path>
                    void iterate(Node &n, const Ftor &ftor, Path &path)
                    {
                        ftor.open(n, path);
                        path.push_back(&n);
                        for (auto &ch: n.childs)
                            iterate(ch, ftor, path);
                        path.pop_back();
                        ftor.close(n, path);
                    }

            }

            template <typename Node, typename Ftor>
                void iterate(Node &n, const Ftor &ftor)
                {
                    typedef std::vector<Node*> Path;
                    Path path;
                    priv::iterate(n, ftor, path);
                }
        }
    }
}

#endif
