module rinle.view.nodeInfo;

import rinle.model.interfaces;
import rinle.model.filesystem;

class Manager
{
    this (bool defaultRecurse = true, bool defaultExpandBeforeShow = true)
        {
            _defaultRecurse = defaultRecurse;
            _defaultExpandBeforeShow = defaultExpandBeforeShow;
        }

    NodeInfo get(INode node)
        {
            NodeInfo nodeInfo;
            NodeInfo* p = (cast(void*)node in _nodeInfo);
            if (p !is null)
                nodeInfo = *p;
            else if (cast(Dir)node !is null)
                nodeInfo = new NodeInfo(_defaultRecurse, false);
            else
                nodeInfo = new NodeInfo(_defaultRecurse, _defaultExpandBeforeShow);
            return nodeInfo;
        }
    void set(NodeInfo ni, INode node)
        {
            _nodeInfo[cast(void*)node] = ni;
        }

private:
    bool _defaultRecurse;
    bool _defaultExpandBeforeShow;
    NodeInfo[void*] _nodeInfo;
}

class NodeInfo
{
    this (bool r, bool ebs)
        {
            recurse = r;
            expandBeforeShow = ebs;
        }

    bool recurse;
    bool expandBeforeShow;
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
