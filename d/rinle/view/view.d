module rinle.view.view;

import rinle.view.nodeInfo;
import rinle.model.interfaces;

import gubg.ui;

import tango.text.convert.Format;

import gubg.puts;

class View
{
    this (INode base)
        {
            _base = base;
            _current = base;
            _mgr = new Manager;
        }

    void show(Output output)
        {
	    auto ft = new FormatTree(Tag.create(_base, Color.white, false));
	    addTo(ft, _base);
            auto collector = new OutputCollector(output);
            setSelected(ft);
            collector(ft);
        }

    void move(char[] dir)
        {
            INode newCurrent;
            switch (dir)
            {
            case "up":
//                _view.moveUp();
                break;
            case "down":
//                _view.moveDown();
                break;
            case "in":
                newCurrent = _current.replaceComponent(ReplaceMode.Get, 0, null);
                break;
            case "out":
                newCurrent = _current.parent;
                if (newCurrent is null)
                    puts("Parent is null");
                break;
            }
            if (newCurrent !is null)
                _current = newCurrent;
        }

private:

    void addTo(inout FormatTree ft, INode node)
	{
	    auto saveFT = ft;
	    // Add this node to the format tree
	    node.addTo(ft);

	    // Recursively descent, if indicated by the corresponding node info
	    if (_mgr.get(node).recurse)
		for (uint i = 0; i < node.nrComponents; ++i)
		    addTo(ft, node.replaceComponent(ReplaceMode.Get, i, null));
	    ft = saveFT;
	}

    void setSelected(FormatTree ft, bool select = false)
        {
            if (ft.tag.node == _current)
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
    Manager _mgr;
}
