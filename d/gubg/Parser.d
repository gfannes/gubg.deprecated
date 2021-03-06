module gubg.Parser;

template TParser()
{
    T parse(T)(inout T obj, char[] buffer)
	{
	    _globalBuffer = buffer.dup;
	    _ixs = [0];
	    prepareParsing;
	    complete!(T)(obj);
	    finishedParsing;
	    return obj;
	}

    char[] buffer()
	{
	    return _globalBuffer[_ixs[$-1] .. $];
	}

    bool matches(char[] s, bool cons = false)
	{
            char[] buf = buffer();
	    if (buf.length < s.length || buf[0 .. s.length] != s)
		return false;
	    if (cons)
		consume(s.length);
	    return true;
	}

    bool before(inout char[] b, char[] s, bool cons = false)
	{
	    char[] buf = buffer;
	    uint matchIX = locatePattern(buf, s);
	    if (matchIX >= buf.length)
		return false;
	    b = buf[0 .. matchIX];
	    if (cons)
		consume(matchIX + s.length);
	    return true;
	}

//   def matches(s, e = nil)
//     case s
//     when String
//       if buffer[0, s.length] == s
//         consume(s.length)
//         yield(s) if block_given?
//         matches(e) if e
//         return true
//       end
//     when Regexp
//       m = s.match(buffer)
//       if m
//         consume(m[0].length)
//         yield(m[m.length-1]) if block_given?
//         matches(e) if e
//         return true
//       end
//     end
//     return false
//   end

    void consume(uint nr)
	{
	    if (_ixs.length <= 1)
		throw new Exception("I cannot consume when I'm not parsing");
	    _ixs[$-1] +=  + nr;
	    for (char[] buf = buffer; buf.length > 0 && isWhitespace(buf[0]); buf = buffer)
		_ixs[$-1] += 1;
	}
    bool isWhitespace(char ch)
	{
	    foreach (c; _whitespaceSymbols)
		if (ch == c)
		    return true;
	    return false;
	}
    void setWhitespaceSymbols(char[] whitespaceSymbols)
	{
	    _whitespaceSymbols.length = whitespaceSymbols.length;
	    _whitespaceSymbols[] = whitespaceSymbols[];
	}
  
    bool complete(U)(inout U obj = null)
	{
	    bool res;
	    _ixs ~= [_ixs[$-1]];
	    res = complete4Parser(obj);
	    if (res)
		_ixs[$-2] = _ixs[$-1];
	    _ixs.length = _ixs.length-1;
	    return res;
	}
    
private:
    char[] _globalBuffer;
    uint[] _ixs;
    char[] _whitespaceSymbols;
}

version (UnitTest)
{
    import tango.text.Util;

    class XMLNode
    {
	this (char[] tag)
	    {
		puts("Creating XMLNode {}", tag);
		_tag = tag;
	    }

	void addNode(XMLNode node)
	    {
		_childs ~= [node];
	    }
	char[] tag(){return _tag;}
	char[] closingTag(){return "</" ~ _tag ~ ">";}
    private:
	char[] _tag;
	XMLNode[] _childs;
    }

    class XMLParser
    {
	mixin TParser;

	void prepareParsing(){}
	void finishedParsing(){}

	// Someone else might have already started the construction of obj. The goal here is to complete its construction
	bool complete4Parser(T)(inout T obj)
	    {
		if (obj is null)
		{
		    if (matches("<html>"))
		    {
			obj = new XMLNode("html");
			consume(6);
		    } else if (matches("<a>"))
		    {
			obj = new XMLNode("a");
			consume(3);
		    } else if (matches("<b>"))
		    {
			obj = new XMLNode("b");
			consume(3);
		    } else return false;
		}

		XMLNode node;
		while (complete(node))
		{
		    obj.addNode(node);
		    node = null;
		}

		if (!matches(obj.closingTag))
		    return false;
		consume(obj.closingTag.length);
		
		return true;
	    }
    }

    import gubg.Puts;

    void main()
    {
	auto parser = new XMLParser;
        XMLNode xml;
	parser.parse(xml, "<html><a></a><b></b></html>");
	puts("xml = {}", xml);
    }
}
