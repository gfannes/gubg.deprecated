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
const bool printCreate = false;

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

    bool isAttribute()
	{
	    foreach (attr; mAttributes)
		switch (attr)
		{
		case "+":
		case "-":
		    return true;
		    break;
		default:
		    break;
		}
	    return false;
	}

    mixin Create!(TS);
    static void createI(inout Attributes res, inout TS ts, in Config config)
	{
	    static char[][] symbols = ["+", "-", "*", "_", "/"];

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

    char[] fullName()
	{
	    return location == "" ? name : location ~ "." ~ name;
	}

    char[] name;
    char[] location;

    mixin Create!(TS);
    static void createI(inout Name res, inout TS ts, in Config config)
	{
	    char[] name;
	    if (ts.getIdentifier(name))
	    {
		char[] ident;
		while (ts.isSymbol(".") && ts.getIdentifier(ident))
		    name ~= "." ~ ident;
		res = new Name(name);
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
	    uint s;
	    foreach (el; mElements)
		if (el.mAttributes.isAttribute)
		    s += el.size();
	    return s;
        }

    void add(Element el)
	{
	    mElements ~= [el];
	}

    Element[] elements(){return mElements;}

    void setLocation(char[] location)
	{
	    foreach (el; mElements)
		el.setLocation(location);
	}

    void print(uint level = 0)
	{
	    foreach (el; mElements)
		el.print(level);
	}

    mixin Create!(TS);
    static void createI(inout Body res, inout TS ts, in Config config)
	{
	    Element[] els;
	    Element el;
	    while ((el = Element.create(ts, config)) !is null)
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

    void setLocation(char[] location)
    {
	mName.location = location;
	auto newLocation = (location == "" ? mName.name : location ~ "." ~ mName.name);
	if (mBody !is null)
	    mBody.setLocation(newLocation);
    }

    void collectBodies(inout Body[char[]] fn2Body)
	{
	    if (mBody !is null)
	    {
		fn2Body[mName.fullName] = mBody;
		foreach (el; mBody.elements)
		    el.collectBodies(fn2Body);
	    }
	}

    void resolveBodies(in Body[char[]] fn2Body)
	{
	    if (mBody !is null)
		foreach (el; mBody.elements)
		    el.resolveBodies(fn2Body);
	    else
	    {
		Body *bdy = (mBodyName.name in fn2Body);
		if (bdy is null)
		    throw new Exception("ERROR::Could not resolve " ~ mBodyName.name);
		mBody = *bdy;
	    }
	}

    uint size()
	{
	    if (mBody !is null)
		return mBody.size();
	    return 0;
	}

    bool depthFirst(bool delegate(Element element) dg)
        {
            if (!dg(this))
                return false;
            if (isGenuine)
                foreach (el; mBody.elements)
                    if (!el.depthFirst(dg))
                        return false;
            return true;
        }

    bool isRoot(){return false;}
    bool isGenuine(){return mBodyName is null;}
    char[] fullName(){return mName.fullName();}

    void print(uint level = 0)
	{
	    if (mBodyName is null)
	    {
		puts("{}{}({}).{} ({})", repeat("  ", level), mAttributes.attributes, mName.location, mName.name, size());
		mBody.print(level + 1);
	    } else
	    {
		puts("{}{}({}).{} -> {} ({})", repeat("  ", level), mAttributes.attributes, mName.location, mName.name, mBodyName.name, size());
	    }
	}

    mixin Create!(TS);
    static void createI(inout Element res, inout TS ts, in Config config)
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
			   (bdy = Body.create(ts, config)) !is null &&
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
            auto config = Config.createFrom(dirName);

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
	    auto bdy = Body.create(ts, config);
            return new Element(attrs, name, bdy);
        }

private:
    Attributes mAttributes;
    Name mName;
    Body mBody;
    Name mBodyName;
}

class Root: Element
{
    this ()
    {
	super(new Attributes([]), new Name("<root>"), new Body([]));
    }

    void add(Element el)
    {
	mBody.add(el);
    }

    void setLocation()
    {
	mName.location = "";
	mBody.setLocation(mName.location);
    }

    bool isRoot(){return true;}
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
