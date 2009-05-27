module rinle.view.view;

import rinle.model.interfaces;
import rinle.view.nodeInfo;
import rinle.view.focusMgr;

import gubg.ui;

import tango.text.convert.Format;

import gubg.puts;

class View: FocusMgr
{
    this (INode base, IUI ui, IFocus defaultFocus)
        {
            _base = base;
            _current = base;
	    _ui = ui;
	    super(_ui);
	    push(defaultFocus);
            _nodeInfoMgr = new NodeInfoMgr;
        }
    ~this ()
	{
	    delete _nodeInfoMgr;
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

    void move(char[] dir)
        {
	    puts("Moving {}", dir);
            INode newCurrent;
            switch (dir)
            {
            case "up":
		uint ix;
		if (FormatTree.indexOfParent!(INode)(_current, ix) && ix > 0)
		    newCurrent = _current.parent.replaceComponent(ReplaceMode.Get, --ix, null);
                break;
            case "down":
		uint ix;
		if (FormatTree.indexOfParent!(INode)(_current, ix) && ix < _current.parent.nrComponents-1)
		    newCurrent = _current.parent.replaceComponent(ReplaceMode.Get, ++ix, null);
                break;
            case "in":
                _current.expand;
		if (_current.nrComponents > 0)
		    newCurrent = _current.replaceComponent(ReplaceMode.Get, 0, null);
                break;
            case "out":
                newCurrent = _current.parent;
                break;
            }
            if (newCurrent !is null)
            {
                _current = newCurrent;
                puts("_current changed to {}", cast(void*)_current);
            } else
                puts("WARNING::newCurrent is null.");
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
    NodeInfoMgr _nodeInfoMgr;
}
