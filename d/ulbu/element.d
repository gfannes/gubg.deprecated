module ulbu.element;

import tango.core.Tuple;
import tango.text.Util;

import gubg.file;
import gubg.parser.tokenSequence;
import gubg.puts;

import ulbu.language;
import ulbu.create;
import ulbu.config;
import ulbu.assembler;

// For the Create mixin
public uint level = 0;
alias TokenSequence!(ULBULanguage) TS;
const bool printCreate = true;

bool readDelimiter(TS ts, bool mustBePresent = false)
{
    bool res = !mustBePresent;
    while (ts.isSymbol(";") || ts.isSymbol("\n"))
	res = true;
    return res;
}

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
    bool isMethod()
	{
	    foreach (attr; mAttributes)
		switch (attr)
		{
		case "*":
		    return true;
		    break;
		default:
		    break;
		}
	    return false;
	}
    bool isDirective()
	{
	    foreach (attr; mAttributes)
		switch (attr)
		{
		case "$":
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
	    static char[][] symbols = ["+", "-", ".", "_", "/", "$"];

            char[][] attrs;
	    char[] symb;
            while (readDelimiter(ts) && ts.isSymbol(symbols, symb))
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
    char[] functionName()
	{
	    return "_uf." ~ fullName;
	}

    char[] name;
    char[] location;

    mixin Create!(TS);
    static void createI(inout Name res, inout TS ts, in Config config)
	{
	    char[] name;
	    if (ts.getIdentifier(name) || ts.getKeyword(name))
	    {
		char[] ident;
		while (ts.isSymbol(".") && (ts.getIdentifier(ident) || ts.getKeyword(ident)))
		    name ~= "." ~ ident;
		res = new Name(name);
		
	    }
	}
}

// class BodyReference
// {
// private:
//     Body* mBody;
// }

// class 

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

    bool isFunction()
        {
            foreach (el; mElements)
                if (el.isMethod)
		    return false;
            return true;
        }

    void print(uint level = 0)
	{
	    foreach (el; mElements)
            {
		el.print(level);
            }
	}

    mixin Create!(TS);
    static void createI(inout Body res, inout TS ts, in Config config)
	{
	    Element[] els;
	    Element el;
	    while ((el = Element.create(ts, config)) !is null)
		els ~= [el];
            if (els.length == 0)
                puts("WARNING::Empty body found");
            res = new Body(els);
	}

private:
    Element[] mElements;
}

class AsmBody: Body
{
    this (AsmElement[] els)
        {
            super(els);
        }

    uint size()
        {
	    return 0;
        }

    void add(AsmElement el)
	{
	    mElements ~= [el];
	}

    Element[] elements(){return cast(AsmElement[])mElements;}

    void setLocation(char[] location)
	{
	    foreach (el; mElements)
		el.setLocation(location);
	}

    bool isFunction()
        {
            return false;
        }

    void print(uint level = 0)
	{
            puts("AsmBody:");
	    foreach (el; mElements)
            {
		el.print(level);
            }
	}

    mixin Create!(TS);
    static void createI(inout AsmBody res, inout TS ts, in Config config)
	{
	    AsmElement[] els;
	    AsmElement el;
	    while ((el = AsmElement.create(ts, config)) !is null)
		els ~= [el];
	    res = new AsmBody(els);
	}
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
	    else if (mBodyName !is null)
	    {
		Body *bdy = (mBodyName.name in fn2Body);
		if (bdy is null)
		    throw new Exception("ERROR::Could not resolve " ~ mBodyName.name);
		mBody = *bdy;
	    } else
            {
                throw new Exception("ERROR::Neither body or body name is present.");
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

    void addFunctionInstructions(AsmFile asmFile)
        {
            foreach (el; mBody.elements)
            {
                el.addFunctionInstructions(asmFile);
            }
        }

    bool isRoot(){return false;}
    bool isGenuine()
        {
            if (mBodyName is null)
            {
                if (mBody is null)
                    throw new Exception("ERROR::No body and no body name present.");
                return true;
            }
            return false;
        }
    char[] name(){return mName.name;}
    char[] fullName(){return mName.fullName();}
    char[] functionName(){return mName.functionName();}
    bool isFunction()
	{
	    return !mAttributes.isAttribute && mBody.isFunction();
	}
    bool isMethod(){return mAttributes.isMethod();}

    void print(uint level = 0)
	{
	    if (mBodyName is null)
	    {
                if (mBody !is null)
                {
                    puts("{}{}({}).{} ({})", repeat("  ", level), mAttributes.attributes, mName.location, mName.name, size());
                    mBody.print(level + 1);
                } else
                {
                }
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
		if (attrs.isDirective)
		{
		    switch (name.name)
		    {
		    case "asm":
			AsmBody asmBody;
			if (readDelimiter(ts) &&
			    ts.isSymbol("{") &&
			    (asmBody = AsmBody.create(ts, config)) !is null &&
			    readDelimiter(ts) &&
			    ts.isSymbol("}"))
			    res = new Element(attrs, name, asmBody);
			break;
		    default:
			throw new Exception("Unknown directive " ~ name.name);
			break;
		    }
		} else if (ts.isSymbol(":") && ts.create(bodyName) && readDelimiter(ts, true))
                {
                    // Reuse of existing body
                    res = new Element(attrs, name, bodyName);
                } else if (readDelimiter(ts) &&
			   ts.isSymbol("{") &&
			   (bdy = Body.create(ts, config)) !is null &&
			   readDelimiter(ts) &&
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
	    readDelimiter(ts);
            if (ts.length != 0)
            {
                puts("ERROR::Unparsed tokens found:");
                while(!ts.empty)
                {
		    char[] str;
		    switch (ts.peep.str)
		    {
		    case "\n":
			str = "\\n";
			break;
		    default:
			str = ts.peep.str;
			break;
		    }
                    putsn("{} ", str);
                    ts.pop;
                }
                throw new Exception("FATAL::Unparsed tokens found");
            }
            return new Element(attrs, name, bdy);
        }

private:
    Attributes mAttributes;
    Name mName;
    Body mBody;
    Name mBodyName;
}

class AsmElement: Element
{
    this (char[] instr, AsmOperand lhs = null, AsmOperand rhs = null)
    {
        super(new Attributes([]), new Name("AsmInstruction"));
        mInstruction = instr;
        mLhs = lhs;
        mRhs = rhs;
    }

    mixin Create!(TS);
    static void createI(inout AsmElement res, inout TS ts, in Config config)
	{
	    readDelimiter(ts);
            AsmOperand lhs, rhs;
            if (ts.isKeyword("movl") && ts.create(lhs) && ts.isSymbol(",") && ts.create(rhs))
                res = new AsmElement("movl", lhs, rhs);
            else if (ts.isKeyword("int") && ts.create(lhs))
                res = new AsmElement("int", lhs, rhs);
        }

    void addFunctionInstructions(AsmFile asmFile)
        {
            asmFile.add(gasCode());
        }

    bool isGenuine(){return false;}
    bool isFunction(){return false;}
    void resolveBodies(in Body[char[]] fn2Body){}

private:

    char[] gasCode()
    {
        if (mLhs is null)
            return mInstruction;
        else if (mRhs is null)
            return mInstruction ~ " " ~ mLhs.gasCode();
        else
            return mInstruction ~ " " ~ mLhs.gasCode() ~ ", " ~ mRhs.gasCode();
    }

    char[] mInstruction;
    AsmOperand mLhs;
    AsmOperand mRhs;
}

class AsmOperand
{
    mixin Create!(TS);
    static void createI(inout AsmOperand res, inout TS ts, in Config config)
	{
            foreach (type; Tuple!(LiteralOperand, RegisterOperand))
            {
                res = type.create(ts);
                if (res !is null)
                    break;
            }
        }
    abstract char[] gasCode();
}

class LiteralOperand: AsmOperand
{
    this (char[] ident)
    {
        mIdent = ident;
    }

    mixin Create!(TS);
    static void createI(inout LiteralOperand res, inout TS ts, in Config config)
	{
            char[] ident;
            if (ts.isSymbol("$") && ts.getIdentifier(ident))
                res = new LiteralOperand(ident);
        }
    
    char[] gasCode()
    {
        return "$" ~ mIdent;
    }

private:
    char[] mIdent;
}

class RegisterOperand: AsmOperand
{
    this (char[] ident)
    {
        mIdent = ident;
    }

    mixin Create!(TS);
    static void createI(inout RegisterOperand res, inout TS ts, in Config config)
	{
            char[] ident;
            if (ts.isSymbol("%") && ts.getIdentifier(ident))
                res = new RegisterOperand(ident);
        }
    
    char[] gasCode()
    {
        return "%" ~ mIdent;
    }

private:
    char[] mIdent;
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
    bool isFunction(){return false;}

    Element main;
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
