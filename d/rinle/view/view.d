module rinle.view.view;

import rinle.model.interfaces;
import rinle.view.nodeInfo;

import gubg.ui;

import tango.text.convert.Format;

import gubg.puts;

class View
{
    this (INode base, IUI ui, bool delegate(inout ICommand command) defaultCommander)
        {
            _base = base;
            _current = base;
	    _ui = ui;
	    _defaultCommander = defaultCommander;
            _nodeInfoMgr = new NodeInfoMgr;
        }
    ~this ()
	{
	    delete _nodeInfoMgr;
	}

    bool create(inout ICommand command)
    {
	// First, we allow _current to handle the request for a command
	if (_current.create(command, _ui, &setCurrent))
	    return true;
	
        // If this fails, we revert to our _defaultCommander
	if (_defaultCommander(command))
	    return true;

        // throw new Exception("Could not handle the request for a command.");

	return false;
    }

    void show(IOutput output)
        {
	    auto ft = new FormatTree(Tag.create(_base, Color.white, false));
	    if (_nodeInfoMgr.get(_current).expandBeforeShow)
                _current.expand;
	    _base.addTo(ft, &_nodeInfoMgr.getFormatInfo);
            auto collector = new OutputCollector(output);
            setSelected(ft);
            collector(ft);
        }

    void move(char[] dir, uint ix = 0)
        {
	    puts("Moving {}", dir);
            INode newCurrent;
            switch (dir)
            {
            case "up":
		if (FormatTree.indexOfParent!(INode)(_current, ix) && ix > 0)
		    newCurrent = _current.parent.replaceComponent(ReplaceMode.Get, --ix, null);
                break;
            case "down":
		if (FormatTree.indexOfParent!(INode)(_current, ix) && ix < _current.parent.nrComponents-1)
		    newCurrent = _current.parent.replaceComponent(ReplaceMode.Get, ++ix, null);
                break;
            case "in":
                _current.expand;
		if (_current.nrComponents > 0)
		    newCurrent = _current.replaceComponent(ReplaceMode.Get, ix, null);
                break;
            case "out":
                newCurrent = _current.parent;
                break;
            }
	    setCurrent(newCurrent);
        }

    void insert(char[] location, IUI ui)
        {
	    puts("Inserting {}", location);
            INode newNode;
            switch (location)
            {
            case "end":
                if (!_current.isLeaf && _current.create(newNode, _current.nrComponents, ui))
                    _current.replaceComponent(ReplaceMode.Create, _current.nrComponents, newNode);
                break;
            case "after":
                break;
            case "before":
                break;
            case "replace":
                break;
            }
        }

private:
    bool setCurrent(INode node)
    {
	if (node !is null)
	{
	    _current = node;
	    puts("_current changed to {}", cast(void*)_current);
	    return true;
	}
	puts("WARNING::node in setCurrent is null.");
	return false;
    }

    void setSelected(FormatTree ft, bool select = false)
        {
            if (ft.tag.node.uid == _current.uid)
                select = true;
            if (select)
            {
                auto tag = ft.tag;
                tag.invert = true;
                ft.setTag(tag);
            }
            for (uint ix = 0; ix < ft.nrComponents; ++ix)
            {
                auto el = ft.replaceComponent(ReplaceMode.Get, ix, null);
                if (!el.isLeaf)
                    setSelected(cast(FormatTree)el, select);
            }
        }

    INode _base;
    INode _current;
    IUI _ui;
    bool delegate(inout ICommand command) _defaultCommander;
    NodeInfoMgr _nodeInfoMgr;
}
