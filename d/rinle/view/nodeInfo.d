module rinle.view.nodeInfo;

import rinle.model.interfaces;
import rinle.model.filesystem;

import gubg.puts;

class NodeInfoMgr
{
    NodeInfo get(INode node)
        {
            NodeInfo nodeInfo;
            if (_current is null)
                throw new Exception("_current is null");
            if (node is null)
                throw new Exception("node is null");
            if (node.uid == _current.uid)
                nodeInfo = new NodeInfo(true, true, true);
            else if (areSiblings(_current, node))
		nodeInfo = new NodeInfo(true, false, false);
	    else
	    {
		uint distance;
		if (distanceToRoot(distance, _current, node))
		{
		    nodeInfo = new NodeInfo(true, true, false);
		} else if (distanceToRoot(distance, node, _current))
		{
		    if (cast(Dir)node !is null)
		    {
			if (distance == 0)
			    nodeInfo = new NodeInfo(true, true, true);
			else
			    nodeInfo = new NodeInfo(true, false, false);
		    } else
		    {
			NodeInfo* p = (cast(void*)node in _nodeInfo);
			if (p !is null)
			    nodeInfo = *p;
			else
			    nodeInfo = new NodeInfo(true, true, true);
		    }
		} else
		{
		    nodeInfo = new NodeInfo(false, false, false);
		}
	    }
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
    void setCurrent(INode current)
	{
	    _current = current;
	}

private:
    NodeInfo[void*] _nodeInfo;
    INode _current;
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
