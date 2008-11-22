module ulbu.builder;

import tango.core.Array;

import ulbu.element;

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

class Builder
{
    void build(TokenSequence!(ULBULanguage) ts)
	{
	    foreach (token; ts)
		puts("token({}) = \"{}\"", token.known, token.str);
	    puts("Nr tokens = {}", ts.length);
	}
}

version (Test)
{
    import gubg.file;
    import gubg.puts;
    
    void main()
    {
	char[] sourceCode;
	loadFile(sourceCode, "data/test.ulbu");
	auto ts = new TokenSequence!(ULBULanguage)(sourceCode);

	auto builder = new Builder;

	builder.build(ts);
    }
}
