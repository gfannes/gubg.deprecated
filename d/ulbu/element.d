module ulbu.element;

import tango.core.Array;

import gubg.parser.language;
import gubg.parser.tokenSequence;
import gubg.parser.token;

import gubg.puts;

class ULBULanguage: Language
{
    static this()
    {
        mSymbols = ["+", "-", ":", "{", "}", "(", ")", ";", ".", "_"];

 	foreach (symb; mSymbols)
	    mIsSymbol[symb] = true;

        mSortedSymbols = mSymbols.dup;
 	sort(mSortedSymbols, delegate bool(char[] lhs, char[] rhs){return lhs.length > rhs.length;});

 	foreach (keyword; mKeywords)
	    mIsKeyword[keyword] = true;
    }

    static char[][] symbols(){return mSymbols;}
    static char[][] sortedSymbols(){return mSortedSymbols;}
    static bool isSymbol(char[] str){return (str in mIsSymbol) !is null;}

    static char[][] keywords(){return mKeywords;}
    static bool isKeyword(char[] str){return (str in mIsKeyword) !is null;}

private:
    static char[][] mSymbols;
    static char[][] mSortedSymbols;
    static bool[char[]] mIsSymbol;

    static char[][] mKeywords;
    static bool[char[]] mIsKeyword;
}

alias TokenSequence!(ULBULanguage) TS;
alias Token!(ULBULanguage) TokenT;

public uint level = 0;
char[] indent()
{
    char[] id;
    id.length = 2*level;
    foreach (ix, inout ch; id)
	ch = (ix%2 == 0 ? '|' : ' ');
    return id;
}

void fail(TS ts)
{
    puts("Failing on the following code:");
    for (uint i = 0; i < 10 && !ts.empty; ++i, ts.pop)
	puts("Element = {}", ts.peep.str);
    throw new Exception("Mmh, some unknown things that start with the above.");
}

char[] createString(TS ts)
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

template Create()
{
    // Create a node based on TS input
    static typeof(this) create(inout TS ts)
	{
	    static TokenT head;

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
}

class ElementAttribute
{
    this (char[] attr)
	{
	    attributes = attr;
	}

    mixin Create;
    static void createI(inout ElementAttribute res, inout TS ts)
	{
	    static char[][] symbols = ["+", "-", ".", "_", "/"];
	    char[] symb;
	    if (ts.isSymbol(symbols, symb))
		res = new ElementAttribute(symb);
	}

private:
    char[] attributes;
}

class ElementName
{
    this (char[] str)
	{
	    name = str;
	}

    char[] name;

    mixin Create;
    static void createI(inout ElementName res, inout TS ts)
	{
	    char[] ident;
	    if (ts.getIdentifier(ident))
		res = new ElementName(ident);
	}
}
