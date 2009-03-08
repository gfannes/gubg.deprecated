module rinle.lines.lines;

import tango.text.Util;

import rinle.interfaces;
import rinle.content;
import rinle.node;
import rinle.sink;

class LineContent: Content
{
    // IContent implementation
    char[] description(){return "LineContent";}
    bool load(void[] content)
    {
	auto lines = splitLines(cast(char[])content);
	foreach (line; lines)
	    addChild(new LineNode(line));
	return true;
    }
    bool save(ref void[] content)
    {
	return true;
    }

    bool show()
    {
	bool b = false;
	foreach (ln; childs)
	    b |= ln.show;
	return b;
    }

    void render(Sink sink)
    {
 	if (show)
	    foreach (ix, ch; childs)
		ch.render(sink);
    }
}

class LineNode: Node
{
    this(char[] line)
    {
	foreach (wn; WordNode.split(line))
	    addChild(wn);
    }

    bool show()
    {
	bool b = super.show;
	if (!b)
	    foreach (wn; childs)
		b |= wn.show;
	return b;
    }

    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	auto h = sink.create(tag);
	foreach (ch; childs)
	    ch.render(h);
	h.newline;
    }

    INode navigate(Direction dir)
    {
	INode res = super.navigate(dir);
	if (cast(LineContent)res)
	    res = res.parent;
	return res;
    }

private:
}

class WordNode: Node
{
    this (char[] word){mWord = word;}

    static WordNode[] split(char[] line)
    {
	auto words = delimit(line, " \t");
	auto wns = new WordNode[words.length];
	foreach (ix, word; words)
	    wns[ix] = new WordNode(word);
	return wns;
    }

    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	auto h = sink.create(tag, mWord);
    }

private:
    char[] mWord;
}

version (UnitTest)
{
    void main()
    {
	auto lc = new LineContent();
	lc.load("abc\ndef");
    }
}