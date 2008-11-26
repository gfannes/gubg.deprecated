module ulbu.element;

import tango.text.Util;

import gubg.file;
import gubg.parser.tokenSequence;
import gubg.puts;

import ulbu.language;
import ulbu.create;
import ulbu.config;
import ulbu.builder;

// For the Create mixin
public uint level = 0;
alias TokenSequence!(ULBULanguage) TS;

class Attributes
{
    this (char[][] attr)
	{
	    mAttributes = attr;
	}

    char[] attributes()
	{
	    return join(mAttributes, "|");
	}

    mixin Create!(TS);
    static void createI(inout Attributes res, inout TS ts, inout Builder builder)
	{
	    static char[][] symbols = ["+", "-", ".", "_", "/"];

            char[][] attrs;
	    char[] symb;
            while (ts.isSymbol(symbols, symb))
                attrs ~= [symb];
            res = new Attributes(attrs);
	}

    char[][] mAttributes;
}

class Name
{
    this (char[] str)
	{
	    name = str;
	}

    void setLocation(char[] loc)
	{
	    location = loc;
	}

    char[] name;
    char[] location;

    mixin Create!(TS);
    static void createI(inout Name res, inout TS ts, inout Builder builder)
	{
	    char[] ident;
	    if (ts.getIdentifier(ident))
	    {
		res = new Name(ident);
//		res.setLocation(loc);
	    }
	}
}

class Body
{
    this (Element[] els)
        {
            mElements = els;
        }

    uint size()
        {
            return -1;
        }

    void print(uint level = 0)
	{
	    foreach (el; mElements)
		el.print(level);
	}

    mixin Create!(TS);
    static void createI(inout Body res, inout TS ts, inout Builder builder)
	{
	    Element[] els;
	    Element el;
	    while ((el = Element.create(ts, builder)) !is null)
		els ~= [el];
	    res = new Body(els);
	}

private:
    Element[] mElements;
}

class Element
{
    this (Attributes attrs, Name name)
        {
            mAttributes = attrs;
            mName = name;
        }

    this (Attributes attrs, Name name, Body bdy)
        {
            this(attrs, name);
            mBody = bdy;
        }

    this (Attributes attrs, Name name, Name bodyName)
        {
            this(attrs, name);
            mBodyName = bodyName;
        }

    Body resolvedBody()
        {
            if (mBody is null)
                throw new Exception("ERROR::Could not resolve the body for " ~ mName.name ~ " which has bodyName " ~ mBodyName.name);
            return mBody;
        }

    void resolveBody(inout Builder builder)
        {
            mBody = builder.resolveBody(mBodyName.name);
        }

    uint size()
        {
            return resolvedBody.size();
        }

    void print(uint level = 0)
	{
	    if (mBody !is null)
	    {
		puts("{}{}({}).{} ({})", repeat("  ", level), mAttributes.attributes, mName.location, mName.name, size());
		mBody.print(level + 1);
	    } else
	    {
		puts("{}{}({}).{} -> {} ({})", repeat("  ", level), mAttributes.attributes, mName.location, mName.name, mBodyName.name, size());
	    }
	}

    mixin Create!(TS);
    static void createI(inout Element res, inout TS ts, inout Builder builder)
	{
            Attributes attrs;
            Name name, bodyName;
            Body bdy;
            if (ts.create(attrs) && ts.create(name))
            {
                if (ts.isSymbol(":") && ts.create(bodyName))
                {
                    // Reuse of existing body
                    res = new Element(attrs, name, bodyName);
                } else if (ts.isSymbol("{") &&
			   (bdy = Body.create(ts, builder.addLocation(name.name))) !is null &&
			   ts.isSymbol("}"))
                {
                    // New body definition
                    res = new Element(attrs, name, bdy);
                }
            }
	}

    static Element createFrom(char[] dirName, char[] fileName, inout Builder builder)
        {
	    // Get the configuration
            auto conf = Config.createFrom(dirName);

	    // Create the name and attributes for this element
	    auto elName = split(fileName,".")[0];
	    auto name = new Name(elName);
	    auto attrs = new Attributes([]);
            // Create the element body based on the token sequence
            // Load the source file
            char[] sourceCode;
            loadFile(sourceCode, dirName ~ "/" ~ fileName);
            // Create its token sequence
            auto ts = new TS(sourceCode);
	    auto bdy = Body.create(ts, builder.setLocation(conf.location(elName)));
            return new Element(attrs, name, bdy);
        }

private:
    Attributes mAttributes;
    Name mName;
    Body mBody;
    Name mBodyName;
}

version (Test)
{
    import gubg.puts;
    
    void main()
    {
        auto el = Element.createFrom("test", "test.ulbu");
	el.print();
    }
}
