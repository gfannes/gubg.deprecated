module ulbu.element;

import tango.text.Util;

import gubg.file;
import gubg.parser.tokenSequence;
import gubg.puts;

import ulbu.language;
import ulbu.create;
import ulbu.config;

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
    static void createI(inout Attributes res, inout TS ts, in char[] loc)
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
    static void createI(inout Name res, inout TS ts, in char[] loc)
	{
	    char[] ident;
	    if (ts.getIdentifier(ident))
	    {
		res = new Name(ident);
		res.setLocation(loc);
	    }
	}
}

class Body
{
    this (Element[] els)
        {
            mElements = els;
        }

    void print(uint level = 0)
	{
	    foreach (el; mElements)
		el.print(level);
	}

    mixin Create!(TS);
    static void createI(inout Body res, inout TS ts, in char[] loc)
	{
	    Element[] els;
	    Element el;
	    while ((el = Element.create(ts, loc)) !is null)
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

    void print(uint level = 0)
	{
	    if (mBody !is null)
	    {
		puts("{}{}({}).{}", repeat("  ", level), mAttributes.attributes, mName.location, mName.name);
		mBody.print(level + 1);
	    } else
	    {
		puts("{}{}({}).{} -> {}", repeat("  ", level), mAttributes.attributes, mName.location, mName.name, mBodyName.name);
	    }
	}

    mixin Create!(TS);
    static void createI(inout Element res, inout TS ts, in char[] loc)
	{
            Attributes attrs;
            Name name, bodyName;
            Body bdy;
            if (ts.create(attrs) && ts.create(name))
            {
		name.setLocation(loc);
                if (ts.isSymbol(":") && ts.create(bodyName))
                {
                    // Reuse of existing body
                    res = new Element(attrs, name, bodyName);
                } else if (ts.isSymbol("{") &&
			   (bdy = Body.create(ts, loc ~ "." ~ name.name)) !is null &&
			   ts.isSymbol("}"))
                {
                    // New body definition
                    res = new Element(attrs, name, bdy);
                }
            }
	}

    static Element createFrom(char[] dirName, char[] fileName)
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
	    auto bdy = Body.create(ts, conf.location(elName));
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
