module rinle.model.prog.parser;

import rinle.model.prog.prog;

import gubg.puts;
import gubg.parser;

import tango.text.Util;
import tango.core.Tuple;

class ProgParser
{
    mixin TParser;

    void prepareParsing(){}
    void finishedParsing(){}

    bool complete4Parser(T: ProgNode)(inout T obj)
	{
 	    scope l = new Log("complete4Parser for ProgNode");

	    if (obj !is null)
		return false;

// 	    {
// 		ProgIdentifier progID;
// 		if (complete(progID))
// 		{
// 		    obj = progID;
// 		    return true;
// 		}
// 	    }
	    foreach (type; Tuple!(ProgIdentifier,
				  ProgSymbol))
	    {
		type tmp;
		if (complete(tmp))
		{
		    obj = tmp;
		    return true;
		}
	    }
	    
	    return false;
	}

    bool complete4Parser(T: ProgModule)(inout T obj)
	{
 	    scope l = new Log("complete4Parser for ProgModule");

	    if (obj is null)
		return false;
	    ProgNode node;
	    while (complete(node))
	    {
                l.puts("node = {}", cast(void*)node);
		obj.replaceComponent(ReplaceMode.Create, obj.nrComponents, node);
		node = null;
	    }
	    return true;
	}

    bool complete4Parser(T: ProgIdentifier)(inout T obj)
	{
 	    scope l = new Log("complete4Parser for ProgIdentifier");

	    if (obj !is null)
		return false;
	    auto buf = buffer;
	    uint nr = 0;
	    foreach (ch; buf)
	    {
		if (('a' <= ch && ch <= 'z') ||('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9') || ch == '_')
		    ++nr;
		else
		    break;
	    }
	    if (nr == 0)
		return false;
	    l.puts("Found identifier \"" ~ buf[0 .. nr] ~ "\"");
	    obj = new ProgIdentifier(buf[0 .. nr]);
	    consume(nr);

	    return true;
	}

    bool complete4Parser(T: ProgSymbol)(inout T obj)
	{
	    static char[] symbols = ".,;:!?=|%&*+-/~\\'\"{}()[]<>";
	    
 	    scope l = new Log("complete4Parser for ProgSymbol");

	    if (obj !is null)
		return false;
	    auto buf = buffer;
	    uint nr = 0;
	    foreach (ch; buf)
	    {
		bool found = false;
		foreach (symb; symbols)
		    if (ch == symb)
		    {
			++nr;
			found = true;
			break;
		    }
		if (!found)
		    break;
	    }
	    if (nr == 0)
		return false;
	    l.puts("Found symbol \"" ~ buf[0 .. nr] ~ "\"");
	    obj = new ProgSymbol(buf[0 .. nr]);
	    consume(nr);

	    return true;
	}

    bool complete4Parser(T: ProgScope)(inout T obj)
	{
 	    scope l = new Log("complete4Parser for ProgScope");

	    if (obj !is null)
		return false;

	    if (matches("{", true))
	    {
		obj = new ProgScope;
		ProgNode node;
		while (complete(node))
		{
		    obj.replaceComponent(ReplaceMode.Create, obj.nrComponents, node);
		    decl = null;
		}
		if (matches("}", true))
		    return true;
		obj = null;
	    }

	    return false;
	}
}

version (UnitTest)
{
    import gubg.file;
    void main()
    {
        char[] fileName = "parser.d";

	char[] content;
	loadFile(content, fileName);

	auto parser = new ProgParser;
	auto mod = new ProgModule("./", fileName);
	parser.parse(mod, content);
    }
}
