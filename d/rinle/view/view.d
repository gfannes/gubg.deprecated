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
	    puts("show: _base = {}, _current = {}, _current.parent = {}", cast(void*)_base, cast(void*)_current, cast(void*)_current.parent);
	    auto ft = new FormatTree(Tag.create(_base, Color.white, false));
	    if (_mgr.get(_current).expandBeforeShow)
                _current.expand;
	    _base.addTo(ft, &_mgr.getFormatInfo);
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
		if (FormatTree.indexOfParent(_current, ix) && ix > 0)
		    newCurrent = _current.parent.replaceComponent(ReplaceMode.Get, --ix, null);
                break;
            case "down":
		uint ix;
		if (FormatTree.indexOfParent(_current, ix) && ix < _current.parent.nrComponents-1)
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
	    puts("_current = {}, newCurrent = {}, _current.parent = {}", cast(void*)_current, cast(void*)newCurrent, cast(void*)_current.parent);
            if (newCurrent !is null)
                _current = newCurrent;
        }

private:
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
