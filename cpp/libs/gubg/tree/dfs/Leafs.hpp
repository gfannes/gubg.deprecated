#ifndef HEADER_gubg_tree_dfs_Leafs_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_dfs_Leafs_hpp_ALREADY_INCLUDED

#include <vector>

namespace gubg
{
    namespace tree
    {
        namespace dfs
        {
            namespace priv
            {
                template <typename Node, typename Leafs>
                    void addLeaf(Node &node, Leafs &leafs)
                    {
                        if (node.childs.empty())
                        {
                            leafs.push_back(&node);
                            return;
                        }
                        for (auto &ch: node.childs)
                            addLeaf(*ch, leafs);
                    }
            }
            template <typename Node>
                std::vector<Node*> leafs(Node &node)
                {
                    std::vector<Node*> res;
                    priv::addLeaf(node, res);
                    return res;
                }
        }
    }
}

#endif
