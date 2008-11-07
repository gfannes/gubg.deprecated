module rinle.d.declaration;

import gubg.iofiber;
import rinle.d.d;
import rinle.globals;
import rinle.interfaces;
import rinle.d.parser.token;
import rinle.d.parser.tokenSequence;
import rinle.d.statement;
import rinle.sink;
import tango.core.Tuple;

import gubg.puts;

class DDeclaration: DStatement
{
    this()
    {
	addSemicolon = true;
    }

    mixin Create;
    static void createI(inout DDeclaration res){}
    static void createI(inout DDeclaration res, inout Environment env)
    {
	res = DDeclaration.createFrom(env.askString("Declaration: "));
    }
    static void createI(inout DDeclaration res, inout TokenSequence ts)
    {
	foreach (type; Tuple!(DAttributeDeclaration,
                              DImportDeclaration,
                              DVersionDeclaration,
                              DClassDeclaration,
                              DFunctionDeclaration,
                              DVariableDeclaration))
	{
	    res = type.create(ts);
	    if (res !is null)
		break;
	}
    }

    static DDeclaration createFrom(char[] str)
    {
	DDeclaration decl;
	switch (str)
	{
	case "version":
	    decl = DVersionDeclaration.create(env);
	    break;
	case "import":
	    decl = DImportDeclaration.create(env);
	    break;
	case "class":
	    decl = DClassDeclaration.create(env);
	    break;
	case "fun":
	    decl = DFunctionDeclaration.create(env);
	    break;
	case "var":
	    decl = DVariableDeclaration.create(env);
	    break;
	case "int":
	case "uint":
	case "long":	    
	    decl = DVariableDeclaration.createFrom(str);
	    break;
	default:
	    break;
	}

	return decl;
    }
}

class DAttributeDeclaration: DDeclaration
{
    static this()
    {
	attributes = ["public", "private", "auto", "scope", "abstract"];
    }

    this ()
    {
	setNrChilds(1);
    }
    this (char[] attr, DDeclaration decl = null)
    {
	this();
	setChild(0, new DIdentifier(attr));
        if (decl !is null)
            addChild(decl);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = new DIdentifier(env.askString("Attribute: "));
	    break;
        case 1:
            res = DDeclaration.create(env);
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DAttributeDeclaration res){res = new DAttributeDeclaration();}
    static void createI(inout DAttributeDeclaration res, inout Environment env)
    {
	res = DAttributeDeclaration.create();
	res.setChild(0, new DIdentifier(env.askString("The attribute: ")));
	VariantFiber.yield(res);
	res.setChild(1, DDeclaration.createFrom(env.askString("The declaration: ")));
    }
    static void createI(inout DAttributeDeclaration res, inout TokenSequence ts)
    {
        char[] attr;
        DDeclaration decl;
	if (ts.isKeyword(attributes, attr) &&
            ts.create(decl))
	    res = new DAttributeDeclaration(attr, decl);
    }

    void renderI(Sink sink)
    {
        if (childs[0] !is null)
            childs[0].render(sink);
        if (childs[1] !is null)
            childs[1].render(sink);
    }

private:
    static char[][] attributes;
}

class DModuleDeclaration: DDeclaration
{
    this ()
    {
	setNrChilds(1);
    }
    this (char[] name)
    {
	this();
	auto ident = new DIdentifier(name);
	setChild(0, ident);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = new DIdentifier(env.askString("Module: "));
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DModuleDeclaration res){res = new DModuleDeclaration();}
    static void createI(inout DModuleDeclaration res, inout Environment env)
    {
	res = DModuleDeclaration.create();
	VariantFiber.yield(res);
	res.setChild(0, new DIdentifier(env.askString("The module's name: ")));
    }
    static void createI(inout DModuleDeclaration res, inout TokenSequence ts)
    {
	if (ts.isKeyword("module"))
	{
	    char[] name;
	    while (!ts.isSymbol(";"))
	    {
		if (ts.empty)
		    throw new Exception("Could not find closing \";\" for \"module\".");
		name ~= ts.pop.str;
	    }
	    res = new DModuleDeclaration(name);
	}
    }

    void renderI(Sink sink)
    {
	sink.add("module ");
	foreach (ix, ch; childs)
	    if (ch !is null)
		ch.render(sink);
    }
}

// [0]: Import name (DIdentifier)
class DImportDeclaration: DDeclaration
{
    this ()
    {
	setNrChilds(1);
    }
    this (char[] name)
    {
	this();
	auto ident = new DIdentifier(name);
	setChild(0, ident);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = new DIdentifier(env.askString("Import: "));
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DImportDeclaration res)
    {
	res = new DImportDeclaration;
    }
    static void createI(inout DImportDeclaration res, inout Environment env)
    {
	res = DImportDeclaration.create();
	VariantFiber.yield(res);
	res.setChild(0, new DIdentifier(env.askString("The import's name: ")));
    }
    static void createI(inout DImportDeclaration res, inout TokenSequence ts)
    {
	if (ts.isKeyword("import"))
	{
	    char[] name;
	    while (!ts.isSymbol(";"))
	    {
		if (ts.empty)
		    throw new Exception("Could not find closing \";\" for \"import\".");
		name ~= ts.pop.str;
	    }
	    res = new DImportDeclaration(name);
	}
    }

    void renderI(Sink sink)
    {
	sink.add("import ");
	foreach (ix, ch; childs)
	    if (ch)
		ch.render(sink);
    }
}

// [0]: Class name (DIdentifier)
// [1]: Superclass name (DIdentifier)
// [2]: Body (DScope)
class DClassDeclaration: DDeclaration
{
    this ()
    {
	setNrChilds(3);
	addSemicolon = false;
    }
    this (DIdentifier name, DIdentifier parent, DScope bdy)
    {
	this();
	puts("Setting {} and {}", name, parent);
	setChild(0, name);
	setChild(1, parent);
	setChild(2, bdy);
    }
    this (char[] name, char[] parent = "")
    {
	this(new DIdentifier(name), (parent != "" ? new DIdentifier(parent) : null), new DScope);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	case 1:
	    res = new DIdentifier(env.askString((ix == 0 ? "Class name: " : "Base class: ")));
	    break;
	case 2:
	    res = new DScope;
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DClassDeclaration res)
    {
	res =  new DClassDeclaration(null, null, new DScope);
    }
    static void createI(inout DClassDeclaration res, inout Environment env)
    {
	res = DClassDeclaration.create();
	VariantFiber.yield(res);
	res.setChild(0, new DIdentifier(env.askString("The class's name: ")));
	VariantFiber.yield(res);
	res.setChild(1, new DIdentifier(env.askString("The parent's name: ")));
    }
    static void createI(inout DClassDeclaration res, inout TokenSequence ts)
    {
	char[] className;
	if (ts.isKeyword("class") && ts.getIdentifier(className))
	{
	    char[] baseName;
	    if (ts.isSymbol(":"))
		if (!ts.getIdentifier(baseName))
		    return;

	    DScope scop;
	    if (createDNode(scop, ts))
		res = new DClassDeclaration(new DIdentifier(className), new DIdentifier(baseName), scop);
	}
    }

    void renderI(Sink sink)
    {
	sink.add("class ");
	if (childs[0] !is null)
	    childs[0].render(sink);
	if (childs[1] !is null)
	{
	    sink.add(": ");
	    childs[1].render(sink);
	}
	sink.newline;
	childs[2].render(sink);
    }
}

// [0]: (DIdent)
// [1]: DScope DVersion: DStat
class DVersionDeclaration: DDeclaration
{
    this (char[] name)
    {
	auto ident = new DIdentifier(name);
	addChild(ident);
	auto scop = new DScope;
	addChild(scop);
	addSemicolon = false;
    }
    this (DIdentifier ident, DScope scop)
    {
	addChild(ident);
	addChild(scop);
	addSemicolon = false;
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = new DIdentifier(env.askString("Version: "));
	    break;
	case 1:
	    res = new DScope;
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DVersionDeclaration res){}
    static void createI(inout DVersionDeclaration res, inout Environment env)
    {
	res = new DVersionDeclaration(env.askString("Version: "));
    }

    static void createI(inout DVersionDeclaration res, inout TokenSequence ts)
    {
	Token test;
	DScope scop;
	if (ts.isKeyword("version") &&
	    ts.isSymbol("(") &&
	    (test = ts.pop) !is null &&
	    ts.isSymbol(")") &&
	    createDNode(scop, ts))
	{
	    res = new DVersionDeclaration(new DIdentifier(test.str), scop);
	}
    }

    void renderI(Sink sink)
    {
	sink.add("version (");
	childs[0].render(sink);
	sink.add(")");
	sink.newline;
	childs[1].render(sink);
    }
}

class DInitializer: DIdentifier
{
    // For now, this is just an identifier
    this (char[] init)
    {
	super(init);
    }

    mixin Create;
    static void createI(inout DInitializer res){}
    static void createI(inout DInitializer res, inout Environment env)
    {
	char[] init = env.askString("Initializer: ");
	if (init.length > 0)
	    res = new DInitializer(init);
    }
    static void createI(inout DInitializer res, inout TokenSequence ts)
    {
	char[] ident;
	if (ts.getIdentifier(ident))
	    res = new DInitializer(ident);
    }
}

class DTypeName: DNode
{
    this (char[] typ, char[] name)
    {
	addChild(new DType([], typ));
	addChild(new DIdentifier(name));
    }
    this (DType typ, DIdentifier name)
    {
	addChild(typ);
	addChild(name);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = new DType([], env.askString("Type: "));
	    break;
	case 1:
	    res = new DIdentifier(env.askString("Name: "));
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DTypeName res){}
    static void createI(inout DTypeName res, inout Environment env)
    {
	res =  new DTypeName(env.askString("Type: "), env.askString("Name: "));
    }
    static void createI(inout DTypeName res, inout TokenSequence ts)
    {
	DType typ;
	DIdentifier name;
	if ((typ = DType.create(ts)) !is null &&
	    (name = DIdentifier.create(ts)) !is null)
	    res = new DTypeName(typ, name);
    }

    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	tag.indent = false;
	auto h = sink.create(tag);

	childs[0].render(h);
	h.add(" ");
	childs[1].render(h);
    }
}

class DArgumentTypes: DNode
{
    this(){}
    this (char[][] types, char[][] names)
    {
	foreach (ix, typ; types)
	{
	    DTypeName tn = new DTypeName(typ, names[ix]);
	    addChild(tn);
	}
    }
    this (DTypeName[] typNames)
    {
	foreach (tn; typNames)
	    addChild(tn);
    }

    void createChild(inout INode res, uint ix)
    {
	res = DTypeName.create(env);
    }

    mixin Create;
    static void createI(inout DArgumentTypes res){}
    static void createI(inout DArgumentTypes res, inout Environment env){}
    static void createI(inout DArgumentTypes res, inout TokenSequence ts)
    {
	DTypeName[] typeNames;
	if (ts.isSymbol("("))
	{
	    while (true)
	    {
		DTypeName typName;
		if ((typName = DTypeName.create(ts)) !is null)
		    typeNames ~= [typName];
		else
		{
		    if (ts.isSymbol(","))
			continue;
		    else if (ts.isSymbol(")"))
			res = new DArgumentTypes(typeNames);
		    break;
		}
	    }
	}
    }

    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	tag.indent = false;
	auto h = sink.create(tag);

	h.add("(");
        foreach (ix, child; childs)
        {
            if (ix > 0)
                h.add(", ");
            child.render(h);
        }
	h.add(")");
    }
}

class DFunctionDeclaration: DDeclaration
{
    this (char[] returnType, char[] name)
    {
	auto retT = new DTypeName(returnType, name);
	addChild(retT);
	addSemicolon = false;
    }
    this (DTypeName returnTypeName, DArgumentTypes argTypes = null, DScope bdy = null)
    {
	addChild(returnTypeName);
	if (argTypes is null)
	    argTypes = new DArgumentTypes();
	addChild(argTypes);
	if (bdy is null)
	    bdy = new DScope();
	addChild(bdy);
	addSemicolon = false;
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = DTypeName.create(env);
	    break;
	case 1:
	    res = DArgumentTypes.create(env);
	    break;
	case 2:
	    res = new DScope;
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DFunctionDeclaration res){}
    static void createI(inout DFunctionDeclaration res, inout Environment env)
    {
	res = new DFunctionDeclaration(DTypeName.create);
    }
    
    static void createI(inout DFunctionDeclaration res, inout TokenSequence ts)
    {
	DTypeName typname;
	DArgumentTypes argTypes;
	DScope bdy;
	if ((typname = DTypeName.create(ts)) !is null &&
	    (argTypes = DArgumentTypes.create(ts)) !is null &&
	    createDNode(bdy, ts))
	{
	    res = new DFunctionDeclaration(typname, argTypes, bdy);
	}
    }

    void renderI(Sink sink)
    {
	childs[0].render(sink);
	childs[1].render(sink);
	sink.newline;
	childs[2].render(sink);
    }
}

class DNameInit: DNode
{
    this (DIdentifier name, DInitializer init)
    {
	addChild(name);
	addChild(init);
    }

    mixin Create;
    static void createI(inout DNameInit res){}
    static void createI(inout DNameInit res, inout Environment env){}
    static void createI(inout DNameInit res, inout TokenSequence ts)
    {
	DIdentifier name;
	DInitializer init;
	if (ts.create(name))
	{
	    if (ts.isSymbol("="))
		ts.create(init);
	    res = new DNameInit(name, init);
	}
    }

    void render(Sink sink)
    {
	childs[0].render(sink);
	if (childs[1] !is null)
	{
	    sink.add(" = ");
	    childs[1].render(sink);
	}
    }
}

class DVariableDeclaration: DDeclaration
{
    this (DType typ, DNameInit nameInit)
    {
	addChild(typ);
	addChild(nameInit);
	addSemicolon = true;
    }

    static DVariableDeclaration createFrom(char[] typ)
    {
	DType t = new DType([], typ);
	return DVariableDeclaration.createFrom(t);
    }
    static DVariableDeclaration createFrom(DType t)
    {
	DIdentifier name = new DIdentifier(env.askString("Variable name: "));
	DInitializer init = DInitializer.create(env);
	return new DVariableDeclaration(t, new DNameInit(name, init));
    }

    mixin Create;
    static void createI(inout DVariableDeclaration res){}
    static void createI(inout DVariableDeclaration res, inout Environment env)
    {
	DType t = DType.create(env);
	res = DVariableDeclaration.createFrom(t);
    }

    static void createI(inout DVariableDeclaration res, inout TokenSequence ts)
    {
	DType typ;
	DNameInit nameInit;
	if (ts.create(typ) && ts.create(nameInit) && ts.isSymbol(";"))
	    res = new DVariableDeclaration(typ, nameInit);
    }

    void renderI(Sink sink)
    {
	childs[0].render(sink);
	sink.add(" ");
	childs[1].render(sink);
    }
}
