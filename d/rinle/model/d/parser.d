module rinle.model.d.parser;

import rinle.model.d.d;

import gubg.parser;

import tango.text.Util;

class DParser
{
    mixin TParser;

    void prepareParsing(){}
    void finishedParsing(){}

    bool complete4Parser(T: DModule)(inout T obj)
	{
 	    scope l = new Log("complete4Parser for DModule");

	    if (obj is null)
		return false;
	    DDeclaration decl;
	    while (complete(decl))
	    {
		obj.replaceComponent(ReplaceMode.Create, obj.nrComponents, decl);
		decl = null;
	    }
	    return true;
	}

    bool complete4Parser(T: DDeclaration)(inout T obj)
	{
	    scope l = new Log("complete4Parser for DDeclaration");

	    if (obj !is null)
		return false;
	    
	    if (matches("module", true))
	    {
		char[] name;
		if (!before(name, ";", true))
		    throw new Exception("Could not find the module name.");
		l.puts("Found module with name \"" ~ name ~ "\"");
		obj = new DModuleDeclaration(name);
	    } else if (matches("import", true))
	    {
		char[] name;
		if (!before(name, ";", true))
		    throw new Exception("Could not find the import name.");
		l.puts("Found import with name \"" ~ name ~ "\"");
		obj = new DImportDeclaration(name);
	    } else if (matches("class", true))
	    {
		l.puts("Found class");
//		obj = new DClassDeclaration(name);
	    } else
	    {
		l.puts("Found something else starting with " ~ buffer[0 .. 10]);
		throw new Exception("STOP");
	    }

	    return true;
	}
}

version (UnitTest)
{
    import gubg.puts;
    import gubg.file;
    void main()
    {
	char[] content;
	loadFile(content, "parser.d");

	auto parser = new DParser;
        DModule dmodule = new DModule("./", "parser.d");
	parser.parse(dmodule, content);
    }
}
