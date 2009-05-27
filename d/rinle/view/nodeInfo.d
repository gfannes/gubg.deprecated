module rinle.view.nodeInfo;

import rinle.model.interfaces;
import rinle.model.filesystem;

class NodeInfoMgr
{
    this (bool defaultShow = true, bool defaultRecurse = true, bool defaultExpandBeforeShow = true)
        {
	    _defaultShow = defaultShow;
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
                nodeInfo = new NodeInfo(_defaultShow, _defaultRecurse, true);
            else
                nodeInfo = new NodeInfo(_defaultShow, _defaultRecurse, _defaultExpandBeforeShow);
            return nodeInfo;
        }
    IFormatInfo getFormatInfo(INode node)
	{
	    return get(node);
	}
    void set(NodeInfo ni, INode node)
        {
            _nodeInfo[cast(void*)node] = ni;
        }

private:
    bool _defaultShow;
    bool _defaultRecurse;
    bool _defaultExpandBeforeShow;
    NodeInfo[void*] _nodeInfo;
}

class NodeInfo: IFormatInfo
{
    this (bool s, bool r, bool ebs)
        {
	    _show = s;
            _recurse = r;
            _expandBeforeShow = ebs;
        }

    bool show(){return _show;}
    bool recurse(){return _recurse;}
    bool expandBeforeShow(){return _expandBeforeShow;}

private:
    bool _show;
    bool _recurse;
    bool _expandBeforeShow;
}

version (UnitTest)
{
    import gubg.puts;
    void main()
    {
        auto mgr = new NodeInfoMgr;
        NodeInfo ni;
        ni = mgr.get(null);
        puts("ni = {}", cast(void*)ni);
        ni = mgr.get(null);
        puts("ni = {}", cast(void*)ni);
        ni = mgr.get(null);
        puts("ni = {}", cast(void*)ni);
    }
}
