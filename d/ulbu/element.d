module ulbu.element;

import gubg.file;
import gubg.parser.tokenSequence;
import gubg.puts;

import ulbu.language;
import ulbu.create;

// For the Create mixin
public uint level = 0;
alias TokenSequence!(ULBULanguage) TS;

class Attributes
{
    this (char[][] attr)
	{
	    attributes = attr;
	}

    mixin Create!(TS);
    static void createI(inout Attributes res, inout TS ts)
	{
	    static char[][] symbols = ["+", "-", ".", "_", "/"];

            char[][] attrs;
	    char[] symb;
            while (ts.isSymbol(symbols, symb))
                attrs ~= [symb];
            res = new Attributes(attrs);
	}

private:
    char[][] attributes;
}

class Name
{
    this (char[] str)
	{
	    name = str;
	}

    char[] name;

    mixin Create!(TS);
    static void createI(inout Name res, inout TS ts)
	{
	    char[] ident;
	    if (ts.getIdentifier(ident))
		res = new Name(ident);
	}
}

class Type
{
    this (Element[] els)
        {
            mElements = els;
        }

    mixin Create!(TS);
    static void createI(inout Type res, inout TS ts)
	{
	    if (ts.isSymbol("{"))
            {
                Element[] els;
                Element el;
                while (ts.create(el))
                    els ~= [el];
                if (ts.isSymbol("}"))
                    res = new Type(els);
            }
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

    this (Attributes attrs, Name name, Type type)
        {
            this(attrs, name);
            mType = type;
        }

    this (Attributes attrs, Name name, Name typeName)
        {
            this(attrs, name);
            mTypeName = typeName;
        }

    mixin Create!(TS);
    static void createI(inout Element res, inout TS ts)
	{
            Attributes attrs;
            Name name, typeName;
            Type type;
            if (ts.create(attrs) && ts.create(name))
            {
                if (ts.isSymbol(":") && ts.create(typeName))
                {
                    // Reuse of existing type
                    res = new Element(attrs, name, typeName);
                } else if (ts.create(type))
                {
                    // New type definition
                    res = new Element(attrs, name, type);
                }
            }
	}

private:
    Attributes mAttributes;
    Name mName;
    Type mType;
    Name mTypeName;
}

class Module
{
    this (Element[] els)
        {
            mElements = els;
        }

    void setLocation(char[] location)
        {
            mLocation = location;
            foreach (inout el; mElements)
                el.setLocation();
        }

    mixin Create!(TS);
    static void createI(inout Module res, inout TS ts)
	{
            Element[] els;
            Element el;
            while (ts.create(el))
                els ~= [el];
            res = new Module(els);
	}

    static Module createFrom(char[] dirName, char[] fileName)
        {
            // Load the source file
            char[] sourceCode;
            loadFile(sourceCode, dirName ~ "/" ~ fileName);

            // Create its token sequence
            auto ts = new TS(sourceCode);

            // Create the module based on the token sequence
            return Module.create(ts);
        }

private:
    Element[] mElements;
    char[] mLocation;
}

version (Test)
{
    import gubg.puts;
    
    void main()
    {
        auto mod = Module.createFrom("test", "test.ulbu");
    }
}
