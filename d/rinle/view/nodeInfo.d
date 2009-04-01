module rinle.view.nodeInfo;

import rinle.model.interfaces;

class Manager
{
    this (bool defaultRecurse = true)
        {
            mDefaultRecurse = defaultRecurse;
        }

    NodeInfo get(INode node)
        {
            NodeInfo nodeInfo;
            NodeInfo* p = (cast(void*)node in mNodeInfo);
            if (p !is null)
                nodeInfo = *p;
            else
                nodeInfo = new NodeInfo(mDefaultRecurse);
            return nodeInfo;
        }
    void set(NodeInfo ni, INode node)
        {
            mNodeInfo[cast(void*)node] = ni;
        }

private:
    bool mDefaultRecurse;
    NodeInfo[void*] mNodeInfo;
}

class NodeInfo
{
    this (bool r)
        {
            recurse = r;
        }

    bool recurse;
}

version (UnitTest)
{
    import gubg.puts;
    void main()
    {
        auto mgr = new Manager;
        NodeInfo ni;
        ni = mgr.get(null);
        puts("ni = {}", cast(void*)ni);
        ni = mgr.get(null);
        puts("ni = {}", cast(void*)ni);
        ni = mgr.get(null);
        puts("ni = {}", cast(void*)ni);
    }
}
