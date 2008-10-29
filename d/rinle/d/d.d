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
		puts("{}Trying {}", indent, this.stringof);
		
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
		if (ts.create(ds))
		    res.addChild(ds);
		else
                {
		    if (ts.isSymbol("}"))
			break;
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

/*
 	test!(DModuleDeclaration)("module asdfasdf; STOP", "STOP");
	test!(DModuleDeclaration)("module asdfasdf;", "");

	test!(DImportDeclaration)("import asdfasdf; STOP", "STOP");
	test!(DImportDeclaration)("import asdfasdf;", "");

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

	test!(DVariableDeclaration)("public uint level = 0;", "");

	test!(DFunctionDeclaration)("char[] indent(){char[] id; id.length = 2*level;}", "");
	test!(DExpressionStatement)("id.length = 2*level;", "");
	test!(DScope)("{id.length = 2*level;}", "");

	test!(DForeachStatement)("foreach (inout ch; id){}", "");
	test!(DForeachStatement)("foreach (inout ch; id) ch = 123;", "");
	test!(DForeachStatement)("foreach (inout ch; id) ch = ' ';", "");

	test!(DReturnStatement)("return id;", "");

	test!(DFunctionDeclaration)("char[] indent(){char[] id; id.length = 2*level;foreach (inout ch; id) ch = ' ';return id;} STOP", "STOP");
	test!(DFunctionDeclaration)("void fail(TokenSequence ts){puts(\"Failing on the following code:\");} STOP", "STOP");
  

	test!(DExpression)("a;", ";");
*/
	test!(DExpression)("a ? b : c;", ";");

/*
	test!(DSymbolExpression)("++a;", ";");
	test!(DExpression)("a++;", ";");
*/
//	test!(DForStatement)("for (uint i = 0; i < 10 && !ts.empty; ++i, ts.pop)puts(\"Element = {}\", ts.peep.str);", "");
//     throw new Exception("Mmh, some unknown things that start with the above.");

	puts("{} tests failed out of {}", nrFailed, nrTotal);

// 	auto content = TFile("d.d").read;
// 	auto dcontent = new DContent;
// 	dcontent.load(content);
    }
}
