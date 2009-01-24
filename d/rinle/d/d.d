module rinle.d.d;

import tango.core.Tuple;

import rinle.globals;
import rinle.content;
import rinle.node;
import rinle.d.parser.tokenSequence;
public import rinle.d.parser.token;
import rinle.d.declaration;
import rinle.d.statement;
import rinle.d.expression;

import gubg.puts;

public uint level = 0;
char[] indent()
{
    char[] id;
    id.length = 2*level;
    foreach (ix, inout ch; id)
	ch = (ix%2 == 0 ? '|' : ' ');
    return id;
}

void fail(TokenSequence ts)
{
    puts("Failing on the following code:");
    for (uint i = 0; i < 10 && !ts.empty; ++i, ts.pop)
	puts("Element = {}", ts.peep.str);
    throw new Exception("Mmh, some unknown things that start with the above.");
}

char[] createString(TokenSequence ts)
{
    char[] res;
    scope sp = ts.savePoint;
    for (uint i = 0; i < 10 && !ts.empty; ++i, ts.pop)
    {
	if (i > 0)
	    res ~= " ";
	res ~= ts.peep.str;
    }
    sp.restore;
    return res;
}

class DContent: Content
{
    // IContent
    char[] description(){return "DContent";}
    bool load(void[] content)
    {
	auto ts = new TokenSequence(cast(char[])content);

	INode node = DModuleDeclaration.create(ts);

 	if (node !is null)
 	    addChild(node);
	TokenSequence.Element prevHead;
	while (!ts.empty && (ts.head != prevHead))
	{
	    prevHead = ts.head;

	    node = DDeclaration.create(ts);
	    if (node !is null)
		addChild(node);
	}
	if (prevHead is ts.head)
	    fail(ts);
	
	return true;
    }
    bool save(ref void[] content)
    {
	return true;
    }

    void createChild(inout INode node, uint ix)
    {
	puts("&node = {}", &node);
	if (ix == 0)
	    node = DModuleDeclaration.create(env);
	else
	    node = DDeclaration.create(env);
    }

    void render(Sink sink)
    {
	foreach (ix, ch; childs)
	    ch.render(sink);
    }
}

abstract class DNode: Node
{
    void renderI(Sink sink){}

    void createChild(inout INode node, uint ix)
    {
	puts("createChild(node, ix = {}) is not implemented for this = {}", ix, this);
	return null;
    }

    static bool createDNode(T)(inout T res, inout TokenSequence ts)
    {
	return ((res = cast(T)T.create(ts)) !is null);
    }

    // Mixin this template and declare the following functions:
    //  * static void createI(inout NodeT node) -> create an empty node
    //  * static void createI(inout NodeT node, inout Environment env) -> create a node using user input
    //  * static void createI(inout NodeT node, inout TokenSequence ts) -> create a node using TokenSequence input
    template Create()
    {
	// Create an empty node
	static typeof(this) create()
	    {
		typeof(this) res;
		
		typeof(this).createI(res);
		
		return res;
	    }

	// Create a node based on TokenSequence input
	static typeof(this) create(inout TokenSequence ts)
	    {
		static Token head;

		++level;
		scope (exit) --level;
		puts("{}Trying {} \"{}\"", indent, this.stringof, createString(ts));
		
		typeof(this) res;

		if (head !is ts.peep)
		{
		    head = ts.peep;

		    scope sp = ts.savePoint;
		    
		    typeof(this).createI(res, ts);
		    
		    if (res is null)
			sp.restore;
		    else
			puts("{}OK {}", indent, this.stringof);
		    head = null;
		}
		
		return res;
	    }

	// Create a node based on user input from the environment
	static typeof(this) create(inout Environment env)
	    {
		typeof(this) res;
		
		typeof(this).createI(res, env);
		
		return res;
	    }
    }

    template Extend(FromT)
    {
	// Extend from FromT to typeof(this)
	static void extend(inout typeof(this) io, inout TokenSequence ts)
	    {
		++level;
		scope (exit) --level;
		puts("{}Extending {} to {}", indent, FromT.stringof, this.stringof);
		
		scope sp = ts.savePoint;

		typeof(this) res;
		FromT from = cast(FromT)io;
		
		if (from !is null)
		    extendI(res, from, ts);
		
		if (res is null)
		    sp.restore;
		else
		{
		    puts("{}OK {}", indent, this.stringof);
		    io = res;
		}
	    }
    }

    template Comment()
    {
        mixin Create;
        static void createI(inout typeof(this) res){res = new typeof(this);}
        static void createI(inout typeof(this) res, inout Environment env)
            {
                res = typeof(this).create();
                 res.mComment = "//" ~ env.askString("Comment: ");
            }
        static void createI(inout typeof(this) res, inout TokenSequence ts)
            {
                char[] comment;
                if (ts.getComment(comment))
                {
                    res = typeof(this).create();
                    res.mComment = comment;
                }
            }

        void render(Sink sink)
            {
                Tag tag;
                tag.node = this;
                tag.color = (select ? 1 : 0);
                tag.indent = false;
                auto h = sink.create(tag, mComment);
            }

    private:
        char[] mComment;
    }
}

class DIdentifier: DNode
{
    this (char[] ident){mIdent = ident;}

    mixin Create;
    static void createI(inout DIdentifier res){}
    static void createI(inout DIdentifier res, inout Environment env)
    {
	res = new DIdentifier(env.askString("Identifier: "));
    }
    static void createI(inout DIdentifier res, inout TokenSequence ts)
    {
	char[] ident;
	if (ts.getIdentifier(ident))
	    res = new DIdentifier(ident);
    }

    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	tag.indent = false;
	auto h = sink.create(tag, mIdent);
    }

private:
    char[] mIdent;
}

class DTypeof: DNode
{
    this ()
    {
	setNrChilds(1);
    }
    this (DExpression exp)
    {
	this();
	setChild(0, exp);
    }

    mixin Create;
    static void createI(inout DTypeof res){}
    static void createI(inout DTypeof res, inout Environment env){}
    static void createI(inout DTypeof res, inout TokenSequence ts)
    {
	DExpression exp;
	if (ts.isKeyword("typeof") &&
	    ts.isSymbol("(") &&
	    ts.create(exp) &&
	    ts.isSymbol(")"))
	    res = new DTypeof(exp);
    }

    void renderI(Sink sink)
    {
	sink.add("typeof(");
	if (childs[0] !is null)
	    childs[0].render(sink);
	sink.add(")");
    }
}

class DType: DNode
{
    this (char[][] modifiers, char[] typ)
    {
	foreach (mod; modifiers)
	    addChild(new DIdentifier(mod));
	addChild(new DIdentifier(typ));
    }

    void createChild(inout INode res, uint ix)
    {
	res = new DIdentifier(env.askString((nrChilds == ix ? "Type: " : "Modifier: ")));
    }

    static bool createType(inout TokenSequence ts, inout char[] typ)
    {
	bool found = false;
	char[] t;
	if (!ts.isKeyword(["void", "int", "uint", "char", "bool"], t))
	{
	    char[] ident;
	    while (ts.getIdentifier(ident))
	    {
		t ~= ident;
		if (ts.isSymbol("."))
		    t ~= ".";
		else
		    break;
	    }
	}
	if (t.length > 0)
	{
	    typ ~= t;
	    found = true;
	    while (ts.isSymbol("*"))
		typ ~= "*";
	    if (ts.isSymbol("["))
	    {
		typ ~= "[";
		if (!ts.isSymbol("]"))
		{
		    found = createType(ts, typ) && ts.isSymbol("]");
		}
		typ ~= "]";
	    }
	    while (ts.isSymbol("*"))
		typ ~= "*";
	}

	return found;
    }

    mixin Create;
    static void createI(inout DType res){}
    static void createI(inout DType res, inout Environment env)
    {
	res = new DType([], env.askString("Type: "));
    }
    static void createI(inout DType res, inout TokenSequence ts)
    {
	char[][] modifiers;
	char[] mod, typ;
	while (true)
	{
	    if (ts.isKeyword(["in", "out", "inout", "ref", "lazy"], mod))
		modifiers ~= [mod];
	    else if (createType(ts, typ))
	    {
		res = new DType(modifiers, typ);
		break;
	    }
	    else
		break;
	}
    }
    
    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	tag.indent = false;
	auto h = sink.create(tag);

	if (nrChilds > 0)
	{
	    childs[0].render(h);
	    for (uint i = 1; i < nrChilds; ++i)
	    {
		h.add(" ");
		childs[i].render(h);
	    }
	}
    }
}

class DScope: DNode
{
    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	tag.indent = true;
	auto h = sink.create(tag);

	h.add("{");
	h.newline;

	foreach (ix, ch; childs)
	    ch.render(h);

	h.add("}");
	h.newline;
    }

    void createChild(inout INode res, uint ix)
    {
	res = DStatement.create(env);
    }

    mixin Create;
    static void createI(inout DScope res){}
    static void createI(inout DScope res, inout Environment env)
    {
    }
    static void createI(inout DScope res, inout TokenSequence ts)
    {
	if (ts.isSymbol("{"))
	{
	    res = new DScope();
	    bool ok = true;
	    while (ok)
	    {
		DStatement ds;
		if (ts.isSymbol("}"))
		    break;
		else if (ts.create(ds))
		    res.addChild(ds);
		else
                {
                    ok = false;
                    res = null;
                }
	    }
	}
    }
}

version (Test)
{
    import tango.io.File: TFile = File;

    uint nrFailed = 0;
    uint nrTotal = 0;

    void test(T)(char[] str, char[] last)
    {
	nrTotal++;
	puts("Testing {} on \"{}\" ... ", typeid(T), str);
	auto ts = new TokenSequence(str);
	Node node = T.create(ts);
	if (node is null || (ts.peep is null && last.length > 0) || (ts.peep !is null && ts.peep.str != last))
	{
	    puts("FAILED", str);
	    nrFailed++;
	} else
	    puts("OK");
	puts("");
    }
    void main()
    {
        // d.d, all, one
        auto testType = "one";
        switch (testType)
        {
        case "d.d":
            auto content = TFile("d.d").read;
            auto dcontent = new DContent;
            dcontent.load(content);
            break;
        case "one":

	    test!(DType)("int", "");
	    test!(DType)("int[]", "");
	    test!(DType)("int*[]", "");
	    test!(DType)("a", "");
	    test!(DType)("a.b", "");
	    test!(DType)("typeof(this)", "");
	    
            puts("{} tests failed out of {}", nrFailed, nrTotal);
            break;
        case "all":
/*
 */
            test!(DModuleDeclaration)("module asdfasdf; STOP", "STOP");
            test!(DModuleDeclaration)("module asdfasdf;", "");
            test!(DImportDeclaration)("import asdfasdf; STOP", "STOP");
            test!(DImportDeclaration)("import asdfasdf;", "");
            test!(DVariableDeclaration)("uint level = 0;", "");
            test!(DAttributeDeclaration)("public uint level = 0;", "");
            test!(DFunctionDeclaration)("char[] indent(){char[] id; id.length = 2*level;}", "");
            test!(DFunctionDeclaration)("char[] indent(){char[] id; id.length = 2*level;foreach (inout ch; id) ch = ' ';return id;} STOP", "STOP");
            test!(DFunctionDeclaration)("void fail(TokenSequence ts){puts(\"Failing on the following code:\");} STOP", "STOP");
	    test!(DFunctionDeclaration)("bool createDNode(T)(inout T res, inout TokenSequence ts){}","");
	    test!(DTemplateDeclaration)("template Create(){}", "");
            test!(DDeclaration)("public import a.b.c.d;", "");

            test!(DExpression)("asdfasdf STOP", "STOP");
            test!(DExpression)("asdfasdf", "");
            test!(DExpression)("asdfasdf() STOP", "STOP");
            test!(DExpression)("asdfasdf()", "");
            test!(DExpression)("asdfasdf(asdf) STOP", "STOP");
            test!(DExpression)("asdfasdf(asdf)", "");
            test!(DExpression)("asdfasdf(asdf,asdfas) STOP", "STOP");
            test!(DExpression)("asdfasdf(asdf,asdfas)", "");
            test!(DExpression)("!asdfasdf STOP", "STOP");
            test!(DExpression)("!asdfasdf", "");
            test!(DExpression)("asdfasdf;", ";");
            test!(DExpression)("a+b+c;", ";");
            test!(DExpression)("a = a+b+c;", ";");
            test!(DExpression)("' ';", ";");
            test!(DExpression)("a;", ";");
            test!(DExpression)("++a;", ";");
            test!(DExpression)("a++;", ";");
            test!(DExpression)("a+b;", ";");
            test!(DExpression)("++a++ + ++b;", ";");
            test!(DExpression)("a ? b : c;", ";");
            test!(DExpression)("a+b ? b : c;", ";");
            test!(DExpression)("a.b;", ";");
            test!(DExpression)("true ? true : false ? false : true;", ";");
            test!(DExpression)("a.b.c", "");
            test!(DExpression)("new Exception(\"Mmh, some unknown things that start with the above.\")", "");
            test!(DExpression)("null ! is null;", ";");
	    test!(DExpression)("a is b", "");

            test!(DForArgument)("(;testsd;) STOP", "STOP");
            test!(DForArgument)("(;testsd;)", "");
            test!(DForStatement)("for (;!test(sdfs);++i) {} STOP", "STOP");
            test!(DForStatement)("for (;!test(sdfs);++i) {}", "");
            test!(DIfStatement)("if (true) {;}", "");
            test!(DIfStatement)("if (true) ;", "");
            test!(DIfStatement)("if (true) {;} else {;}", "");
            test!(DIfStatement)("if (true) ; else ;", "");
            test!(DWhileStatement)("while (true) {;}", "");
            test!(DDoStatement)("do {;} while (true)", "");
            test!(DForeachStatement)("foreach (;asdf) ;", "");
            test!(DExpressionStatement)("id.length = 2*level;", "");
            test!(DForeachStatement)("foreach (inout ch; id){}", "");
            test!(DForeachStatement)("foreach (inout ch; id) ch = 123;", "");
            test!(DForeachStatement)("foreach (inout ch; id) ch = ' ';", "");
            test!(DForStatement)("for (uint i = 0; i < 10 && !ts.empty; ++i, ts.pop)puts(\"Element = {}\", ts.peep.str);", "");
            test!(DReturnStatement)("return id;", "");
            test!(DCommentStatement)("//{id.length = 2*level;}", "");
            test!(DStatement)("puts(\"Element = {}\", ts.peep.str);", "");
            test!(DStatement)("throw new Exception(\"Mmh, some unknown things that start with the above.\");", "");
	    test!(DStatement)("auto ts = new TokenSequence(cast(char[])content);", "");

            test!(DScope)("{id.length = 2*level;}", "");
/*
 */
            puts("{} tests failed out of {}", nrFailed, nrTotal);
            break;
        }
    }
}
