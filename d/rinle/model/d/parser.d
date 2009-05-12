module rinle.model.d.parser;

import rinle.model.d.d;

import gubg.puts;
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

    bool complete4Parser(T: DIdentifier)(inout T obj)
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
	    obj = new DIdentifier(buf[0 .. nr]);
	    consume(nr);

	    return true;
	}

    bool complete4Parser(T: DScope)(inout T obj)
	{
 	    scope l = new Log("complete4Parser for DScope");

	    if (obj !is null)
		return false;

	    if (matches("{", true))
	    {
		obj = new DScope;
		DDeclaration decl;
		while (complete(decl))
		{
		    obj.replaceComponent(ReplaceMode.Create, obj.nrComponents, decl);
		    decl = null;
		}
		if (matches("}", true))
		    return true;
		obj = null;
	    }

	    return false;
	}

    bool complete4Parser(T: DClassDeclaration)(inout T obj)
	{
 	    scope l = new Log("complete4Parser for DClassDeclaration");

	    if (obj is null)
		return false;
	    DIdentifier name;
	    if (!complete(name))
		throw new Exception("Could not find the class name.");
	    obj.setName(name);
	    if (matches(":", true))
	    {
		auto baseClasses = new DBaseClasses;
		DIdentifier baseClass;
		while (complete(baseClass))
		{
		    baseClasses.replaceComponent(ReplaceMode.Create, baseClasses.nrComponents, baseClass);
		    baseClass = null;
		    if (!matches(",", true))
			break;
		}
		obj.replaceComponent(ReplaceMode.Create, obj.nrComponents, baseClasses);
	    }
	    DScope scop;
	    if (!complete(scop))
		throw new Exception("Could not find the class' body.");
	    obj.setBody(scop);
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
	    } else if (matches("mixin", true))
	    {
		char[] name;
		if (!before(name, ";", true))
		    throw new Exception("Could not find the mixin name.");
		l.puts("Found mixin with name \"" ~ name ~ "\"");
		obj = new DMixinDeclaration(name);
	    } else if (matches("class", true))
	    {
		l.puts("Found class");
		auto classDecl = new DClassDeclaration;
		complete(classDecl);
		obj = classDecl;
	    } else
	    {
                // Remove this else clause when debugging is over
                char[] buf = buffer();
                if (buf.length > 10)
                {
                    buf = buf[0 .. 10] ~ "...";
                }
		l.puts("WARNING::Could not create DDeclaration from \"" ~ buf ~ "\"");
	    }

	    return (obj !is null);
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
