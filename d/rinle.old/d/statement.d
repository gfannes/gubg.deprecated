module rinle.d.statement;

import tango.core.Tuple;
import tango.text.Util;

import rinle.d.d;
import rinle.globals;
import rinle.interfaces;
import rinle.sink;
import rinle.d.parser.tokenSequence;
import rinle.d.declaration;
import rinle.d.expression;

import gubg.puts;

class DStatement: DNode
{
    this()
    {
	addSemicolon = true;
	addNewline = true;
    }

    mixin Create;
    static void createI(inout DStatement stat){}
    static void createI(inout DStatement stat, inout Environment env)
    {
	char[] str = env.askString("Statement: ");
	switch (str)
	{
	case "for":
	    stat = DForStatement.create(env);
	    break;
	case "if":
	    stat = DIfStatement.create(env);
	    break;
	case "while":
	    stat = DWhileStatement.create(env);
	    break;
	case "do":
	    stat = DDoStatement.create(env);
	    break;
// 	case "foreach":
// 	    stat = DForeachStatement.create(env);
// 	    break;
	case "return":
	    stat = DReturnStatement.create(env);
	    break;
	case "throw":
	    stat = DThrowStatement.create(env);
	    break;
	case "exp":
	    stat = DExpressionStatement.create(env);
	    break;
	default:
	    stat = DDeclaration.createFrom(str);
	    if (stat is null)
		stat = DExpressionStatement.createFrom(str);
	    break;
	}
    }
    static void createI(inout DStatement res, inout TokenSequence ts)
    {
	foreach (type; Tuple!(DCommentStatement,
                              DBlockStatement,
			      DForStatement,
			      DIfStatement,
			      DWhileStatement,
			      DDoStatement,
			      DForeachStatement,
			      DReturnStatement,
			      DThrowStatement,
			      DExpressionStatement,
			      DDeclaration,
			      DEmptyStatement))
	{
	    res = type.create(ts);
	    if (res !is null)
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

	renderI(h);

	if (addSemicolon)
	{
	    h.add(";");
	    if (addNewline)
		h.newline;
	}
    }

    bool addSemicolon;
    bool addNewline;
}

class DCommentStatement: DStatement
{
    this(){}

    mixin Comment;
}

class DEmptyStatement: DStatement
{
    this()
    {
	addSemicolon = true;
    }

    mixin Create;
    static void createI(inout DEmptyStatement res){}
    static void createI(inout DEmptyStatement res, inout Environment env){}
    static void createI(inout DEmptyStatement res, inout TokenSequence ts)
    {
	if (ts.isSymbol(";"))
	    res = new DEmptyStatement();
    }
}

class DBlockStatement: DStatement
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

    mixin Create;
    static void createI(inout DBlockStatement res){}
    static void createI(inout DBlockStatement res, inout Environment env){}
    static void createI(inout DBlockStatement res, inout TokenSequence ts)
    {
	if (ts.isSymbol("{"))
	{
	    res = new DBlockStatement();
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

class DForArgument: DNode
{
    this(DStatement exp1, DExpression exp2, DExpression exp3)
    {
	addChild(exp1);
	addChild(exp2);
	addChild(exp3);
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = DStatement.create(env);
	    break;
	case 1:
	case 2:
	    res = DExpression.create(env);
	    break;
	default:
	    break;
	}
    }

    mixin Create;
    static void createI(inout DForArgument res){}
    static void createI(inout DForArgument res, inout Environment env){}
    static void createI(inout DForArgument res, inout TokenSequence ts)
    {
	DStatement stat;
	DExpression test, increment;
	if (ts.isSymbol("("))
	{
	    if ((ts.create(stat)))
	    {
		ts.create(test);
		if (ts.isSymbol(";"))
		{
		    ts.create(increment);
		    if (ts.isSymbol(")"))
			res = new DForArgument(stat, test, increment);
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

	// Overrule the newline settings
	DStatement stat = cast(DStatement)childs[0];
	bool nl = stat.addNewline;
	stat.addNewline = false;
	stat.render(h);
	stat.addNewline = nl;

	h.add(" ");
	childs[1].render(h);
	h.add("; ");
	childs[2].render(h);
	h.add(")");
    }
}

class DForStatement: DStatement
{
    this(DForArgument forArg, DStatement bdy)
    {
	addChild(forArg);
	addChild(bdy);
	addSemicolon = false;
    }

    mixin Create;
    static void createI(inout DForStatement res){}
    static void createI(inout DForStatement res, inout Environment env)
    {
	res = new DForStatement(new DForArgument(new DEmptyStatement, new DEmptyExpression, new DEmptyExpression), new DBlockStatement);
    }
    static void createI(inout DForStatement res, inout TokenSequence ts)
    {
	DForArgument forArg;
	DStatement bdy;
	if (ts.isKeyword("for") &&
	    ts.create(forArg) &&
	    ts.create(bdy))
	    res = new DForStatement(forArg, bdy);
    }

    void renderI(Sink sink)
    {
	sink.add("for ");
	childs[0].render(sink);
	sink.newline;
	childs[1].render(sink);
    }
}

// class DForeachArgument: DNode
// {
//     this(DType exp1, DExpression exp2, DExpression exp3)
//     {
// 	addChild(exp1);
// 	addChild(exp2);
// 	addChild(exp3);
//     }

//     void createChild(inout INode res, uint ix)
//     {
// 	switch (ix)
// 	{
// 	case 0:
// 	    res = DStatement.create(env);
// 	    break;
// 	case 1:
// 	case 2:
// 	    res = DExpression.create(env);
// 	    break;
// 	default:
// 	    break;
// 	}
//     }

//     static DForeachArgument create()
//     {
// 	return null;
//     }

//     static DForeachArgument create(inout TokenSequence ts)
//     {
// 	++level;
// 	scope (exit) --level;
// 	puts("{}Trying DForeachArgument", indent);

// 	DForeachArgument res;
// 	scope sp = ts.savePoint;

// 	DStatement stat;
// 	DExpression test, increment;
// 	if (ts.isSymbol("("))
// 	{
// 	    if ((ts.create(stat)))
// 	    {
// 		ts.create(test);
// 		if (ts.isSymbol(";"))
// 		{
// 		    ts.create(increment);
// 		    if (ts.isSymbol(")"))
// 			res = new DForeachArgument(stat, test, increment);
// 		}
// 	    }
// 	}

// 	if (res is null)
// 	    sp.restore;
// 	else
// 	    puts("{}OK DForeachArgument", indent);

// 	return res;
//     }

//     void render(Sink sink)
//     {
// 	Tag tag;
// 	tag.node = this;
// 	tag.color = (select ? 1 : 0);
// 	tag.indent = false;
// 	auto h = sink.create(tag);

// 	h.add("(");

// 	// Overrule the newline settings
// 	DStatement stat = cast(DStatement)childs[0];
// 	bool nl = stat.addNewline;
// 	stat.addNewline = false;
// 	stat.render(h);
// 	stat.addNewline = nl;

// 	h.add(" ");
// 	childs[1].render(h);
// 	h.add("; ");
// 	childs[2].render(h);
// 	h.add(")");
//     }
// }

// class DForeachStatement: DStatement
// {
//     this(DForeachArgument forArg, DStatement bdy)
//     {
// 	addChild(forArg);
// 	addChild(bdy);
// 	addSemicolon = false;
//     }

//     static DForeachStatement create()
//     {
// 	return new DForeachStatement(new DForeachArgument(new DEmptyStatement, new DEmptyExpression, new DEmptyExpression), new DBlockStatement);
//     }

//     static DForeachStatement create(inout TokenSequence ts)
//     {
// 	++level;
// 	scope (exit) --level;
// 	puts("{}Trying DForeachStatement", indent);

// 	DForeachStatement res;
// 	scope sp = ts.savePoint;

// 	DForeachArgument forArg;
// 	DStatement bdy;
// 	if (ts.isKeyword("for") &&
// 	    ts.create(forArg) &&
// 	    ts.create(bdy))
// 	    res = new DForeachStatement(forArg, bdy);

// 	if (res is null)
// 	    sp.restore;
// 	else
// 	    puts("{}OK DForeachStatement", indent);

// 	return res;
//     }

//     void renderI(Sink sink)
//     {
// 	sink.add("for ");
// 	childs[0].render(sink);
// 	sink.newline;
// 	childs[1].render(sink);
//     }
// }

class DIfStatement: DStatement
{
    this(DExpression ifArg, DStatement bdy, DStatement elseBdy)
    {
	addChild(ifArg);
	addChild(bdy);
	if (elseBdy !is null)
	    addChild(elseBdy);
	addSemicolon = false;
    }

    mixin Create;
    static void createI(inout DIfStatement res){}
    static void createI(inout DIfStatement res, inout Environment env){}
    static void createI(inout DIfStatement res, inout TokenSequence ts)
    {
	DExpression ifArg;
	DStatement bdy;
	DStatement elseBdy;
	if (ts.isKeyword("if") &&
	    ts.isSymbol("(") &&
	    ts.create(ifArg) &&
	    ts.isSymbol(")") &&
	    ts.create(bdy))
	{
	    if (ts.isKeyword("else"))
		ts.create(elseBdy);
	    res = new DIfStatement(ifArg, bdy, elseBdy);
	}
    }
}

class DWhileStatement: DStatement
{
    this(DExpression whileArg, DStatement bdy)
    {
	addChild(whileArg);
	addChild(bdy);
	addSemicolon = false;
    }

    mixin Create;
    static void createI(inout DWhileStatement res){}
    static void createI(inout DWhileStatement res, inout Environment env){}
    static void createI(inout DWhileStatement res, inout TokenSequence ts)
    {
	DExpression whileArg;
	DStatement bdy;
	if (ts.isKeyword("while") &&
	    ts.isSymbol("(") &&
	    ts.create(whileArg) &&
	    ts.isSymbol(")") &&
	    ts.create(bdy))
	    res = new DWhileStatement(whileArg, bdy);
    }
}

class DDoStatement: DStatement
{
    this(DExpression doArg, DStatement bdy)
    {
	addChild(doArg);
	addChild(bdy);
	addSemicolon = false;
    }

    mixin Create;
    static void createI(inout DDoStatement res){}
    static void createI(inout DDoStatement res, inout Environment env){}
    static void createI(inout DDoStatement res, inout TokenSequence ts)
    {
	DExpression doArg;
	DStatement bdy;
	if (ts.isKeyword("do") &&
	    ts.create(bdy) &&
	    ts.isKeyword("while") &&
	    ts.isSymbol("(") &&
	    ts.create(doArg) &&
	    ts.isSymbol(")"))
	    res = new DDoStatement(doArg, bdy);
    }
}

class DForeachTypeName: DIdentifier
{
    this (char[] typeName)
    {
	super(typeName);
    }

    mixin Create;
    static void createI(inout DForeachTypeName res){}
    static void createI(inout DForeachTypeName res, inout Environment env)
    {
	res =  new DForeachTypeName(env.askString("Name (might include type): "));
    }
    static void createI(inout DForeachTypeName res, inout TokenSequence ts)
    {
	char[][] typeName;
	char[] tmp;
	while (true)
	{
	    if (ts.getKeyword(tmp))
	    {
		typeName ~= tmp;
	    } else
	    {
		if (ts.getIdentifier(tmp))
		{
		    typeName ~= tmp;
		    tmp = join(typeName, " ");
		    res = new DForeachTypeName(tmp);
		} else
		    fail(ts);
		break;
	    }
	}
    }
}

class DForeachArgument: DNode
{
    this (DForeachTypeName[] typeNames, DExpression aggregate)
    {
	switch (typeNames.length)
	{
	case 1:
	    addChild(null);
	    addChild(typeNames[0]);
	    break;
	case 2:
	    addChild(typeNames[0]);
	    addChild(typeNames[1]);
	    break;
	default:
	    addChild(null);
	    addChild(null);
	    break;
	}
	addChild(aggregate);
    }

    mixin Create;
    static void createI(inout DForeachArgument res){}
    static void createI(inout DForeachArgument res, inout Environment env)
    {
	res = new DForeachArgument([], new DEmptyExpression);
    }
    static void createI(inout DForeachArgument res, inout TokenSequence ts)
    {
	if (ts.isSymbol("("))
	{
	    DForeachTypeName[] tns;
	    while (!ts.isSymbol(";"))
	    {
		DForeachTypeName tn;
		if (!ts.create(tn))
		    return;
		tns ~= [tn];
		ts.isSymbol(",");
	    }
	    DExpression aggregate;
	    if (ts.create(aggregate) && ts.isSymbol(")"))
		res = new DForeachArgument(tns, aggregate);
	}
    }
}

class DForeachStatement: DStatement
{
    this (DForeachArgument foreachArg, DStatement bdy)
    {
	addChild(foreachArg);
	addChild(bdy);
	addSemicolon = false;
    }

    mixin Create;
    static void createI(inout DForeachStatement res){}
    static void createI(inout DForeachStatement res, inout Environment env){}
    static void createI(inout DForeachStatement res, inout TokenSequence ts)
    {
	DForeachArgument foreachArg;
	DStatement bdy;
	if (ts.isKeyword("foreach") &&
	    ts.create(foreachArg) &&
	    ts.create(bdy))
	    res = new DForeachStatement(foreachArg, bdy);
    }
}

class DExpressionStatement: DStatement
{
    this(DExpression ex)
    {
	addChild(ex);
	addSemicolon = true;
    }

    void createChild(inout INode res, uint ix)
    {
	switch (ix)
	{
	case 0:
	    res = DExpression.create(env);
	    break;
	default:
	    break;
	}
    }

    static DExpressionStatement createFrom(char[] str)
    {
	return new DExpressionStatement(DExpression.createFrom(str));
    }

    mixin Create;
    static void createI(inout DExpressionStatement res){}
    static void createI(inout DExpressionStatement res, inout Environment env)
    {
	res = new DExpressionStatement(DExpression.create(env));
    }
    static void createI(inout DExpressionStatement res, inout TokenSequence ts)
    {
	DExpression ex;
	if (ts.create(ex) && ts.isSymbol(";"))
	    res = new DExpressionStatement(ex);
    }

    void renderI(Sink sink)
    {
	childs[0].render(sink);
    }
}

class DReturnStatement: DStatement
{
    this(DExpression exp = null)
    {
	if (exp !is null)
	    addChild(exp);
	addSemicolon = true;
    }

    mixin Create;
    static void createI(inout DReturnStatement res){}
    static void createI(inout DReturnStatement res, inout Environment env){}
    static void createI(inout DReturnStatement res, inout TokenSequence ts)
    {
	DExpression exp;
	if (ts.isKeyword("return"))
	{
	    ts.create(exp);
	    if (ts.isSymbol(";"))
		res = new DReturnStatement(exp);
	}
    }
}

class DThrowStatement: DStatement
{
    this(DExpression exp)
    {
	addChild(exp);
	addSemicolon = true;
    }

    mixin Create;
    static void createI(inout DThrowStatement res){}
    static void createI(inout DThrowStatement res, inout Environment env){}
    static void createI(inout DThrowStatement res, inout TokenSequence ts)
    {
	DExpression exp;
	if (ts.isKeyword("throw"))
	{
	    ts.create(exp);
	    if (ts.isSymbol(";"))
		res = new DThrowStatement(exp);
	}
    }
}
