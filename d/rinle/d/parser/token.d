module rinle.d.parser.token;

import tango.core.Array;

import rinle.common.helper;
import gubg.puts;

class Token
{
    char[] str;

    this(){}
    this (char[] s)
	{
	    str = s;
	}

    Token create(inout char[] s){return null;}

    bool known(){return false;}

    bool isKeyword(){return false;}
    bool isKeyword(char[] kw){return false;}
    bool isKeyword(char[][] kws){return false;}
    bool getKeyword(inout char[] kw)
    {
	if (!isKeyword)
	    return false;
	kw = str.dup;
	return true;
    }

    bool isSymbol(){return false;}
    bool isSymbol(char[] symb){return false;}
    bool isSymbol(char[][] symbs){return false;}
    bool getSymbol(inout char[] symb)
    {
	if (!isSymbol)
	    return false;
	symb = str.dup;
	return true;
    }

    bool isIdentifier(){return false;}
    bool isIdentifier(char[] s){return false;}
    bool getIdentifier(inout char[] s)
	{
	    if (!isIdentifier)
		return false;
	    s = str.dup;
	    return true;
	}

    bool isString(){return false;}
    bool getString(inout char[] s)
	{
	    if (!isString)
		return false;
	    s = str.dup;
	    return true;
	}

    bool isComment(){return false;}
    bool getComment(inout char[] s)
	{
	    if (!isComment)
		return false;
	    s = str.dup;
	    return true;
	}

    void print()
        {
            puts("str = {}", str);
        }
}

class String: Token
{
    this(){}
    this (char[] s){super(s);}

    bool known(){return true;}

    bool isString(){return true;}

    String create(inout char[] s)
    {
	String res;

	if (s.length >= 2)
	{
	    char c = s[0];
	    if (c == '"' || c == '\'')
	    {
		int ixS = 1, ixEnd;
		do
		{
		    if (ixS >= s.length)
		    {
			ixEnd = -1;
			break;
		    }
		    int ixT, ix;
		    ix = findFirstF(ixT, ["\\", [c]], s[ixS .. $]);
		    if (ixT != 0)
		    {
			ixEnd = (ixT == 1 ? ixS+ix+1 : -1);
			break;
		    }
		    ixS += ix+2;
		} while (true);
		res = new String(s[0 .. ixEnd]);
		s = s[ixEnd .. $];
	    }
	}
	return res;
    }
}

class Comment: Token
{
    this(){}
    this (char[] s){super(s);}

    bool known(){return true;}

    bool isComment(){return true;}

    Comment create(inout char[] s)
    {
	Comment res;
        if (startWith("//", s))
        {
            int ix = findFirstL("\n", s);
            if (ix >= 0)
	    {
                res = new Comment(s[0 .. ix]);
		s = s[ix .. $];
            } else
	    {
                res = new Comment(s);
		s = s[$ .. $];
	    }
        } else if (startWith("/*", s))
	{
            int ix = findFirstL("*/", s);
            if (ix >= 0)
	    {
		res = new Comment(s[0 .. ix]);
		s = s[ix .. $];
	    }
	} else if (startWith("/+", s))
	{
            int nrOpen = 0;
            int ix = 0;
            while (true)
            {
                auto ixTmp = findFirstF("/+", "+/", s[ix .. $]);
                if (ixTmp >= 0)
                {
                    ix += ixTmp;
                    if (startWith("/+", s[ix .. $]))
                    {
                        ix += 2;
                        nrOpen++;
                    } else
                    {
                        ix += 2;
                        nrOpen--;
                        if (nrOpen == 0)
                        {
			    res = new Comment(s[0 .. ix]);
			    s = s[ix .. $];
                            break;
                        }
                    }
                } else
                    break;
            }
	}
        return res;
    }
}

class Symbol: Token
{
    this()
    {
	symbols = ["/", "/=", ".", "..", "...", "&", "&=", "&&", "|", "|=", "||", "-", "-=", "--", "+", "+=", "++", "<", "<=", "<<", "<<=", "<>", "<>=", ">", ">=", ">>=", ">>>=", ">>", ">>>", "!", "!=", "!==", "!<>", "!<>=", "!<", "!<=", "!>", "!>=", "!~", "(", ")", "[", "]", "{", "}", "?", ",", ";", ":", "$", "=", "==", "===", "*", "*=", "%", "%=", "^", "^=", "~", "~=", "~~"];
	sort(symbols, delegate bool(char[] lhs, char[] rhs){return lhs.length > rhs.length;});
    }
    this (char[] s){super(s);}

    bool known(){return true;}

    bool isSymbol()
    {
	return true;
    }

    bool isSymbol(char[] sym)
    {
	return sym == str;
    }

    bool isSymbol(char[][] syms)
    {
	foreach (sym; syms)
	    if (isSymbol(sym))
		return true;
	return false;
    }

    Symbol create(inout char[] s)
    {
	Symbol res;

	foreach (symbol; symbols)
	    if (startWith(symbol, s))
	    {
		res = new Symbol(s[0 .. symbol.length]);
		s = s[symbol.length .. $];
		break;
	    }

	return res;
    }

private:
    char[][] symbols;
}

class Identifier: Token
{
    static bool[char[]] keywords;
    static bool[char[]] symbols;
    static this()
    {
	foreach (keyword; ["abstract", "alias", "align", "asm", "assert", "auto", "body", "bool", "break", "byte", "case", "cast", "catch", "cdouble", "cent", "cfloat", "char", "class", "const", "continue", "creal", "dchar", "debug", "default", "delegate", "delete", "deprecated", "do", "double", "else", "enum", "export", "extern", "false", "final", "finally", "float", "for", "foreach", "foreach_reverse", "function", "goto", "idouble", "if", "ifloat", "import", "in", "inout", "ref", "int", "interface", "invariant", "ireal", "is", "lazy", "long", "mixin", "module", "new", "null", "out", "override", "package", "pragma", "private", "protected", "public", "real", "return", "scope", "short", "static", "struct", "super", "switch", "synchronized", "template", "this", "throw", "true", "try", "typedef", "typeid", "typeof", "ubyte", "ucent", "uint", "ulong", "union", "unittest", "ushort", "version", "void", "volatile", "wchar", "while", "with"])
	    keywords[keyword] = true;
	foreach (symbol; ["is", "in"])
	    symbols[symbol] = true;
    }

    this(){}
    this (char[] s){super(s);}

    bool known(){return true;}

    bool isKeyword()
    {
	return (str in keywords) !is null;
    }
    bool isKeyword(char[] kw)
    {
	return kw == str && isKeyword;
    }
    bool isKeyword(char[][] kws)
    {
	foreach (kw; kws)
	    if (isKeyword(kw))
		return true;
	return false;
    }

    bool isSymbol()
    {
	return (str in symbols) !is null;
    }

    bool isSymbol(char[] sym)
    {
	return sym == str && isSymbol;
    }

    bool isSymbol(char[][] syms)
    {
	foreach (sym; syms)
	    if (isSymbol(sym))
		return true;
	return false;
    }

    bool isIdentifier()
    {
	return !isKeyword;
    }
    bool isIdentifier(char[] s)
    {
	return s == str && isIdentifier;
    }

    Identifier create(inout char[] s)
    {
	return new Identifier(s);
    }
}

version (Test)
{
    void main()
    {
        char[] str;

	Comment comment = new Comment;

        str = "//asdfsadf\nasa/*dfasd*/f";
        auto ct = comment.create(str);
        ct.print();
	puts("rest = ({})", str);

        str = "/*dfa//sd*/f";
        auto bct = comment.create(str);
        bct.print();
	puts("rest = ({})", str);

        str = "/+dfa/+saads+/sd+/f";
        auto nbct = comment.create(str);
        nbct.print();
	puts("rest = ({})", str);

	String string = new String;

        str = "\"sadf\"f";
        auto strin = string.create(str);
	strin.print();
	puts("rest = ({})", str);
    }
}
