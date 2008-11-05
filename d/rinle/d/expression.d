module rinle.d.expression;

import tango.core.Tuple;

import rinle.globals;
import rinle.d.d;
import rinle.interfaces;;
import rinle.sink;
import rinle.d.parser.tokenSequence;
import gubg.iofiber;

import gubg.puts;

class DExpression: DNode
{
    this()
    {
    }

    static DExpression createFrom(char[] str)
    {
	DExpression expr;

// 	if (DKeywordExpression.isKeyword(str))
// 	    expr = new DKeywordExpression(str);
// 	else if (DSymbolExpression.isSymbol(str))
// 	    expr = new DSymbolExpression(str);
// 	else
// 	{
// 	    switch (str)
// 	    {
// 	    case "":
// 		break;
// 	    case "func":
// 		expr = DFunctionExpression.create(env);
// 		break;
// 	    default:
// 		expr = new DFunctionExpression(str);
// 		break;
// 	    }
// 	}

	return expr;
    }

    static DExpression create(inout Environment env)
    {
	return DExpression.createFrom(env.askString("Expression: "));
    }

    static DExpression create(inout TokenSequence ts)
    {
	DExpression res;
	res = DSymbolExpression.create(ts);
// 	// Try to extend the symbol expression to a ternary expression
// 	if (res !is null)
// 	    DTernaryExpression.extend(res, ts);
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

class DSymbolExpression: DExpression
{
    static this()
    {
	symbols = [",", "=", "?", ":", "||", "&&", "+", "-", "/", "*", "~", "<", ">", "<=", ">=", "==", "is", "in"];
	foreach (s; symbols)
	    symbolHash[s] = true;
    }

    this ()
    {
    }

    this (char[] symb, DUnaryExpression ex = null)
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
	    res = DUnaryExpression.create(env);
	    break;
	}
    }

    static bool isSymbol(char[] s)
    {
	return (s in symbolHash) !is null;
    }

    static DSymbolExpression createFrom(char[] str)
    {
	return null;
    }

    static DSymbolExpression createFrom(char[][] symbs, DUnaryExpression[] exps)
    {
	DSymbolExpression res;

	if ((exps.length == symbs.length+1))
	{
            if (symbs.length == 0)
                res = exps[0];
            else
            {
                // Determine the lowest priority symbol found
                char[] symb;
	    all: foreach (s; symbols)
                    foreach (ss; symbs)
                        if (s == ss)
                        {
                            symb = s;
                            break all;
                        }
                // Create the multi expression
                res = new DSymbolExpression(symb);
                // Split the single expressions and add them
		if (symb == "?")
		{
		    uint qmix = -1, ddix = 0;
		    foreach (ix, s; symbs)
		    {
			if (s == "?")
			{
			    if (qmix == -1)
				qmix = ix;
			    else
				++ddix;
			}
			if (s == ":")
			{
			    if (qmix == -1)
				throw new Exception("Found : before ?");
			    if (ddix == 0)
			    {
				ddix = ix;
				break;
			    }
			    --ddix;
			}
		    }
		    res.addChild(DSymbolExpression.createFrom(symbs[0 .. qmix], exps[0 .. qmix+1]));
		    res.addChild(DSymbolExpression.createFrom(symbs[qmix+1 .. ddix], exps[qmix+1 .. ddix+1]));
		    res.addChild(DSymbolExpression.createFrom(symbs[ddix+1 .. $], exps[ddix+1 .. $]));
		} else
		{
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
            }
	}

	return res;
    }

    static DSymbolExpression create()
    {
	return null;
    }

    static DSymbolExpression create(inout TokenSequence ts)
    {
	++level;
	scope (exit) --level;
	puts("{}Trying DSymbolExpression", indent);

	DSymbolExpression res;
	scope sp = ts.savePoint;

	char[][] symbs;
	char[] symb;
	DUnaryExpression[] exps;
	DUnaryExpression exp;
	while (true)
	{
	    if ((exp = DUnaryExpression.create(ts)) is null)
		break;
	    exps ~= [exp];
	    if (!ts.isSymbol(symbols, symb))
		break;
	    symbs ~= [symb];
	}
	res = DSymbolExpression.createFrom(symbs, exps);
	
	if (res is null)
	    sp.restore;
	else
	    puts("{}OK DSymbolExpression", indent);

	return res;
    }
    
    void renderI(Sink sink)
    {
	bool addBrackets = (parent !is null) && (cast(DSymbolExpression)parent !is null);

	if (addBrackets)
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
	if (addBrackets)
	    sink.add(")");
    }

private:
    static char[][] symbols;
    static bool[char[]] symbolHash;
}

class DUnaryExpression: DSymbolExpression
{
    static this()
    {
	symbols = ["!", "++", "--", "-", "+"];
    }

    this ()
    {
	setNrChilds(2);
    }
    this (char[] symb, DPostfixExpression pf)
    {
	this();
	setChild(0, new DIdentifier(symb));
	setChild(0, pf);
    }

    mixin Create;
    static void createI(inout DUnaryExpression res){}
    static void createI(inout DUnaryExpression res, inout Environment env)
    {
    }
    static void createI(inout DUnaryExpression res, inout TokenSequence ts)
    {
	char[] symb;
	if (ts.isSymbol(symbols, symb))
	{
	    DPostfixExpression pf;
	    if (ts.create(pf))
		res = new DUnaryExpression(symb, pf);
	} else
	    res = DPostfixExpression.create(ts);
    }
    
    void renderI(Sink sink)
    {
	if (childs[0])
	    childs[0].render(sink);
	if (childs[1])
	    childs[1].render(sink);
    }

private:
    static char[][] symbols;
}

class DSymbolPostfix: DIdentifier
{
    static this()
    {
	symbols = ["!", "++", "--"];
    }

    this (char[] str)
    {
        super(str);
    }

    mixin Create;
    static void createI(inout DSymbolPostfix res){}
    static void createI(inout DSymbolPostfix res, inout Environment env)
    {
    }
    static void createI(inout DSymbolPostfix res, inout TokenSequence ts)
    {
	char[] symb;
        if (ts.isSymbol(symbols, symb))
            res = new DSymbolPostfix(symb);
    }

private:
    static char[][] symbols;
}

class DIdentifierPostfix: DIdentifier
{
    this (char[] ident)
    {
        super(ident);
    }

    mixin Create;
    static void createI(inout DIdentifierPostfix res){}
    static void createI(inout DIdentifierPostfix res, inout Environment env)
    {
    }
    static void createI(inout DIdentifierPostfix res, inout TokenSequence ts)
    {
        char[] ident;
        if (ts.isSymbol(".") &&
            ts.getIdentifier(ident))
            res = new DIdentifierPostfix(ident);
    }
    
    void renderI(Sink sink)
    {
        sink.add(".");
        super.render(sink);
    }
}

class DArgumentList: DNode
{
    void createChild(inout INode res, uint ix)
    {
	res =  DExpression.create(env);
    }

    mixin Create;
    static void createI(inout DArgumentList res){}
    static void createI(inout DArgumentList res, inout Environment env){}
    static void createI(inout DArgumentList res, inout TokenSequence ts){}

    void createFrom(inout TokenSequence ts)
    {
	while (true)
	{
	    DExpression ds;
	    if (!ts.create(ds))
		return;
	    addChild(ds);
	    if (!ts.isSymbol(","))
		return;
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

class DFunctionPostfix: DArgumentList
{
    mixin Create;
    static void createI(inout DFunctionPostfix res){}
    static void createI(inout DFunctionPostfix res, inout Environment env)
    {
    }
    static void createI(inout DFunctionPostfix res, inout TokenSequence ts)
    {
	if (ts.isSymbol("("))
	{
	    res = new DFunctionPostfix;
	    res.createFrom(ts);
	    if (!ts.isSymbol(")"))
		res = null;
	}
    }
}

// [0]: Postfix or primary expression
// [1]: Postfix itself (symbol, (), [], ...)
class DPostfixExpression: DUnaryExpression
{
    this ()
    {
	setNrChilds(2);
    }
    this (DPostfixExpression ex, DNode postfix)
    {
	setChild(0, ex);
	setChild(1, postfix);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = DPostfixExpression.createFrom(env.askString("Postfix expression: "));
	    break;
	case 1:
	    res = new DIdentifier(env.askString("Postfix symbol: "));
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DPostfixExpression res){res = new DPostfixExpression();}
    static void createI(inout DPostfixExpression res, inout Environment env)
    {
	res = DPostfixExpression.create();
	VariantFiber.yield(res);
	for (uint i = 0; i < 2; ++i)
	{
	    INode ch;
	    res.createChild(ch, i);
	    res.setChild(i, ch);
	    VariantFiber.yield(res);
	}
    }
    static void createI(inout DPostfixExpression res, inout TokenSequence ts)
    {
	res = DPrimaryExpression.create(ts);
	extend(res, ts);
    }
    mixin Extend!(DPrimaryExpression);
    static void extendI(inout DPostfixExpression res, inout DPrimaryExpression from, inout TokenSequence ts)
    {
        while (true)
        {
            DNode postfix;
            foreach (type; Tuple!(DSymbolPostfix,
                                  DFunctionPostfix,
                                  DIdentifierPostfix))
            {
                postfix = type.create(ts);
                if (postfix !is null)
                {
                    if (res is null)
                        res = new DPostfixExpression(from, postfix);
                    else
                        res.addChild(postfix);
                    break;
                }
            }
            if (postfix is null)
                break;
        }
    }
    
    void renderI(Sink sink)
    {
	foreach (ch; childs)
            if (ch !is null)
                ch.render(sink);
    }
}

class DPrimaryExpression: DPostfixExpression
{
    mixin Create;
    static void createI(inout DPrimaryExpression res){}
    static void createI(inout DPrimaryExpression res, inout Environment env)
    {
    }
    static void createI(inout DPrimaryExpression res, inout TokenSequence ts)
    {
	foreach (type; Tuple!(DKeywordExpression,
                              DStringExpression,
			      DIdentifierExpression))
	{
	    res = type.create(ts);
	    if (res !is null)
		break;
	}
    }
}

class DKeywordExpression: DPrimaryExpression
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

class DIdentifierExpression: DPrimaryExpression
{
    this (char[] ident)
    {
	addChild(new DIdentifier(ident));
    }

    mixin Create;
    static void createI(inout DIdentifierExpression res){}
    static void createI(inout DIdentifierExpression res, inout Environment env){}
    static void createI(inout DIdentifierExpression res, inout TokenSequence ts)
    {
	char[] ident;
	if (ts.getIdentifier(ident))
	    res = new DIdentifierExpression(ident);
    }
    
    void renderI(Sink sink)
    {
	childs[0].render(sink);
    }
}

class DStringExpression: DPrimaryExpression
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

/*
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

class DFunctionExpression: DPostfixExpression
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
*/

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

version (Test)
{
    void main()
    {
    }
}
