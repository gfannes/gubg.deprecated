module rinle.model.prog.parser;

import rinle.model.prog.prog;

import gubg.puts;
import gubg.parser;

import tango.text.Util;

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

	    {
		ProgIdentifier progID;
		if (complete(progID))
		    obj = progID;
		return true;
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
 	    scope l = new Log("complete4Parser for DIdentifier");

	    if (obj !is null)
		return false;
	    auto buf = buffer;
	    uint nr = 0;
	    foreach (ch; buf)
	    {
		if (('a' <= ch && ch <= 'z') ||('A' <= ch && ch <= 'Z') || ch == '_')
		    ++nr;
		else
		    break;
	    }
	    l.puts("Found identifier " ~ buf[0 .. nr]);
	    obj = new ProgIdentifier(buf[0 .. nr]);
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
        char[] fileName = "sample.d";

	char[] content;
	loadFile(content, fileName);

	auto parser = new DParser;
        DModule dmodule = new DModule("./", fileName);
	parser.parse(dmodule, content);
    }
}
