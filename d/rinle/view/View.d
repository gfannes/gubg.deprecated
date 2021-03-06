module rinle.view.View;

public import rinle.view.Interfaces;
import rinle.model.Interfaces;
import rinle.view.NodeInfo;

import gubg.UI;

import tango.text.convert.Format;

import gubg.Puts;

class View: IView
{
    this (INode base)
    {
	_nodeInfoMgr = new NodeInfoMgr;
	_base = base;
        setCurrent(base);
	_hide = false;
    }
    ~this ()
    {
	delete _nodeInfoMgr;
    }

    // Methods for IView
    void hide(bool yes)
    {
	_hide = yes;
    }
    void setOutput(IOutput output)
    {
	_output = output;
    }
    void show()
    {
	if (_hide)
	    return;
	auto ft = new FormatTree(Tag.create(_base, Color.white, false));
	if (_nodeInfoMgr.get(_current).expandBeforeShow)
	    _current.expand;
	_base.addTo(ft, &_nodeInfoMgr.getFormatInfo);
	auto collector = new OutputCollector(_output);
	setSelected(ft);
	collector(ft);
    }
    void setCurrent(INode node)
    {
	_current = node;
	_nodeInfoMgr.setCurrent(_current);
	puts("_current changed to {}", cast(void*)_current);
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
    NodeInfoMgr _nodeInfoMgr;

    bool _hide;
    IOutput _output;
}