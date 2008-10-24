module rinle.d.expression;

import tango.core.Tuple;

import rinle.globals;
import rinle.d.d;
import rinle.interfaces;;
import rinle.sink;
import rinle.d.parser.tokenSequence;
import gubg.iofiber;

import gubg.puts;

enum PreviousExpression
{
    unknown,
	multi,
	ternary,
	post
}

class DExpression: DNode
{
    this()
    {
    }

    static DExpression createFrom(char[] str)
    {
	DExpression expr;

	if (DKeywordExpression.isKeyword(str))
	    expr = new DKeywordExpression(str);
	else if (DSymbolExpression.isSymbol(str))
	    expr = new DSymbolExpression(str);
	else
	{
	    switch (str)
	    {
	    case "":
		break;
	    case "func":
		expr = DFunctionExpression.create(env);
		break;
	    default:
		expr = new DFunctionExpression(str);
		break;
	    }
	}

	return expr;
    }

    static DExpression create(inout Environment env)
    {
	return DExpression.createFrom(env.askString("Expression: "));
    }

    static DExpression create(inout TokenSequence ts, PreviousExpression previous = PreviousExpression.unknown)
    {
	DExpression res;
	if (PreviousExpression.ternary != previous)
	{
	    puts("Going for ternary");
	    res = DTernaryExpression.create(ts);
	}
	throw new Exception("asdfasdf");
	if (res is null && PreviousExpression.post != previous)
	    res = DPostExpression.create(ts);
	if (res is null)
	    res = DSymbolExpression.create(ts, PreviousExpression.multi == previous);
	if (res is null)
	    foreach (type; Tuple!(DKeywordExpression, DStringExpression, DFunctionExpression))
	    {
		res = type.create(ts);
		if (res !is null)
		    break;
	    }
	return res;
    }

    void render(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	tag.indent = false;
	auto h = sink.create(tag);

	renderI(h);
    }
}

class DEmptyExpression: DExpression
{
    this()
    {
    }

    static DEmptyExpression create()
    {
	return null;
    }

    void renderI(Sink sink)
    {
	if (select)
	    sink.add("<?>");
    }
}

class DKeywordExpression: DExpression
{
    static this()
    {
	keywords = ["true", "false", "void"];
	foreach (kw; keywords)
	    keywordHash[kw] = true;
    }

    this (char[] keyw)
    {
	addChild(new DIdentifier(keyw));
    }

    static bool isKeyword(char[] kw)
    {
	return (kw in keywordHash) !is null;
    }

    mixin Create;
    static void createI(inout DKeywordExpression res){}
    static void createI(inout DKeywordExpression res, inout Environment env){}
    static void createI(inout DKeywordExpression res, inout TokenSequence ts)
    {
	char[] keyw;
	if (ts.isKeyword(keywords, keyw))
	    res = new DKeywordExpression(keyw);
    }
    
    void renderI(Sink sink)
    {
	childs[0].render(sink);
    }


private:
    static char[][] keywords;
    static bool[char[]] keywordHash;
}

class DStringExpression: DExpression
{
    this (char[] str)
    {
	mString = str;
    }

    mixin Create;
    static void createI(inout DStringExpression res){}
    static void createI(inout DStringExpression res, inout Environment env){}
    static void createI(inout DStringExpression res, inout TokenSequence ts)
    {
	char[] str;
	if (ts.getString(str))
	    res = new DStringExpression(str);
    }
    
    void renderI(Sink sink)
    {
	Tag tag;
	tag.node = this;
	tag.color = (select ? 1 : 0);
	tag.indent = false;
	auto h = sink.create(tag, mString);
    }


private:
    char[] mString;
}

class DFunctionArguments: DNode
{

    void createChild(inout INode res, uint ix)
    {
	res =  DExpression.create(env);
    }

    mixin Create;
    static void createI(inout DFunctionArguments res){}
    static void createI(inout DFunctionArguments res, inout Environment env)
    {
	res = new DFunctionArguments;
    }
    static void createI(inout DFunctionArguments res, inout TokenSequence ts)
    {
	if (ts.isSymbol("("))
	{
	    res = new DFunctionArguments();
	    bool ok = true;
	    while (ok)
	    {
		DExpression ds;
		if (ts.create(ds))
		    res.addChild(ds);
		else
                {
		    if (ts.isSymbol(","))
			continue;
		    if (ts.isSymbol(")"))
			break;
                    ok = false;
                    res = null;
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
	foreach (ix, ch; childs)
	{
	    if (ix > 0)
		h.add(", ");
	    ch.render(h);
	}
	h.add(")");
    }
}

class DFunctionExpression: DExpression
{
    this (char[] varExp, DFunctionArguments args = null)
    {
	addChild(new DIdentifier(varExp));
	if (args !is null)
	    addChild(args);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    char[] str = env.askString("Function name: ");
	    res = new DIdentifier(str);
	    break;
	case 1:
	    res = DFunctionArguments.create(env);
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DFunctionExpression res){}
    static void createI(inout DFunctionExpression res, inout Environment env){}
    static void createI(inout DFunctionExpression res, inout TokenSequence ts)
    {
	DFunctionArguments args;
	char[] varExp;
	char[] ident;
	while (true)
	{
	    if (!ts.getIdentifier(ident))
		break;
	    varExp ~= ident;
	    if (!ts.isSymbol("."))
		break;
	}
	if (varExp.length > 0)
	{
	    ts.create(args);
	    res = new DFunctionExpression(varExp, args);
	}
    }
    
    void renderI(Sink sink)
    {
	childs[0].render(sink);
	if (nrChilds > 1)
	    childs[1].render(sink);
    }
}

class DSymbolExpression: DExpression
{
    static this()
    {
	singleSymbols = ["!", "++", "--", "-", "+"];
	multiSymbols = ["||", "&&", "+", "-", "/", "*", "~", "=", "<", ">", "<=", ">=", "==", "is", "in"];
	foreach (s; singleSymbols)
	    symbolHash[s] = true;
	foreach (s; multiSymbols)
	    symbolHash[s] = true;
    }

    this (char[] symb, DExpression ex = null)
    {
	addChild(new DIdentifier(symb));
	if (ex !is null)
	    addChild(ex);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    char[] str = env.askString("Symbol: ");
	    res = new DIdentifier(str);
	    break;
	default:
	    res = DExpression.create(env);
	    break;
	}
    }

    static bool isSymbol(char[] s)
    {
	return (s in symbolHash) !is null;
    }

    static DSymbolExpression createFrom(char[][] symbs, DExpression[] exps)
    {
	DSymbolExpression res;

	if ((exps.length == symbs.length+1) && symbs.length > 0)
	{
	    // Determine the lowest priority symbol found
	    char[] symb;
	    foreach (s; multiSymbols)
		foreach (ss; symbs)
		    if (s == ss)
		    {
			symb = s;
			break;
		    }
	    // Create the multi expression
	    res = new DSymbolExpression(symb);
	    // Split the single expressions and add them
	    int ixPrev = -1;
	    foreach (ix, s; symbs)
	    {
		if (s == symb)
		{
		    if (ix == ixPrev+1)
			res.addChild(exps[ix]);
		    else
			res.addChild(DSymbolExpression.createFrom(symbs[ixPrev+1 .. ix], exps[ixPrev+1 .. ix+1]));
		    ixPrev = ix;
		}
	    }
	    if (symbs.length == ixPrev+1)
		res.addChild(exps[$-1]);
	    else
		res.addChild(DSymbolExpression.createFrom(symbs[ixPrev+1 .. $], exps[ixPrev+1 .. $]));
	}

	return res;
    }

    static DSymbolExpression create()
    {
	return null;
    }

    static DSymbolExpression create(inout TokenSequence ts, bool single = false)
    {
	++level;
	scope (exit) --level;
	puts("{}Trying DSymbolExpression(single = {})", indent, single);

	DSymbolExpression res;
	scope sp = ts.savePoint;
	bool foundMulti = false;

	// Try a multi-expression if allowed
	if (!single)
	{
	    char[][] symbs;
	    char[] symb;
	    DExpression[] exps;
	    DExpression exp;
	    while (true)
	    {
		if ((exp = DExpression.create(ts, PreviousExpression.multi)) is null)
		    break;
		exps ~= [exp];
		if (!ts.isSymbol(multiSymbols, symb))
		    break;
		symbs ~= [symb];
	    }
	    res = DSymbolExpression.createFrom(symbs, exps);
	}
	// If we do not have a match yet, try a single expression
	if (res is null)
	{
	    sp.restore;
	    char[] symb;
	    DExpression ex;
	    if (ts.isSymbol(singleSymbols, symb))
	    {
		if ((ex = DExpression.create(ts, PreviousExpression.multi)) !is null)
		    res = new DSymbolExpression(symb, ex);
	    }
	} else
	    foundMulti = true;
	
	if (res is null)
	    sp.restore;
	else
	    puts("{}OK DSymbolExpression ({})", indent, (foundMulti ? "multi" : "single"));

	return res;
    }
    
    void renderI(Sink sink)
    {
	bool addBraces = (parent !is null) && (cast(DSymbolExpression)parent !is null);

	if (addBraces)
	    sink.add("(");
	switch (nrChilds)
	{
	case 0:
	    sink.add("<empty symbol expression>");
	    break;
	case 1:
	    childs[0].render(sink);
	    break;
	case 2:
	    childs[0].render(sink);
	    childs[1].render(sink);
	    break;
	default:
	    childs[1].render(sink);
	    for (uint i = 2; i < nrChilds; ++i)
	    {
		sink.add(" ");
		childs[0].render(sink);
		sink.add(" ");
		childs[i].render(sink);
	    }
	    break;
	}
	if (addBraces)
	    sink.add(")");
    }

private:
    static char[][] singleSymbols;
    static char[][] multiSymbols;
    static bool[char[]] symbolHash;
}

class DTernaryExpression: DExpression
{
    this ()
    {
	setNrChilds(3);
    }
    this (DExpression test, DExpression caseTrue, DExpression caseFalse)
    {
	setChild(0, test);
	setChild(1, caseTrue);
	setChild(2, caseFalse);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = DExpression.createFrom(env.askString("Ternary test: "));
	    break;
	case 1:
	    res = DExpression.createFrom(env.askString("Ternary true case: "));
	    break;
	case 2:
	    res = DExpression.createFrom(env.askString("Ternary false case: "));
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DTernaryExpression res){res = new DTernaryExpression();}
    static void createI(inout DTernaryExpression res, inout Environment env)
    {
	res = DTernaryExpression.create();
	VariantFiber.yield(res);
	for (uint i = 0; i < 3; ++i)
	{
	    INode ch;
	    res.createChild(ch, i);
	    res.setChild(i, ch);
	    VariantFiber.yield(res);
	}
    }
    static void createI(inout DTernaryExpression res, inout TokenSequence ts)
    {
	DExpression a, b, c;
	if ((a = DExpression.create(ts, PreviousExpression.ternary)) is null &&
	    ts.isSymbol("?") &&
	    ts.create(b) &&
	    ts.isSymbol(":") &&
	    ts.create(c))
	    res = new DTernaryExpression(a, b, c);
    }
    
    void renderI(Sink sink)
    {
	if (childs[0])
	    childs[0].render(sink);
	sink.add(" ? ");
	if (childs[1])
	    childs[1].render(sink);
	sink.add(" : ");
	if (childs[2])
	    childs[2].render(sink);
    }
}

class DPostExpression: DExpression
{
    static this()
    {
	postSymbols = ["!", "++", "--", "-", "+"];
    }

    this ()
    {
	setNrChilds(2);
    }
    this (DExpression ex, DIdentifier symbol)
    {
	setChild(0, ex);
	setChild(1, symbol);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = DExpression.createFrom(env.askString("Post-symbol expression: "));
	    break;
	case 1:
	    res = new DIdentifier(env.askString("Post-symbol symbol: "));
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DPostExpression res){res = new DPostExpression();}
    static void createI(inout DPostExpression res, inout Environment env)
    {
	res = DPostExpression.create();
	VariantFiber.yield(res);
	for (uint i = 0; i < 2; ++i)
	{
	    INode ch;
	    res.createChild(ch, i);
	    res.setChild(i, ch);
	    VariantFiber.yield(res);
	}
    }
    static void createI(inout DPostExpression res, inout TokenSequence ts)
    {
	DExpression ex;
	char[] symb;
	if ((ex = DExpression.create(ts, PreviousExpression.post)) is null &&
	    ts.isSymbol(postSymbols, symb))
	    res = new DPostExpression(ex, new DIdentifier(symb));
    }
    
    void renderI(Sink sink)
    {
	if (childs[0])
	    childs[0].render(sink);
	if (childs[1])
	    childs[1].render(sink);
    }

private:
    static char[][] postSymbols;
}
