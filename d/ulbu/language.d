module ulbu.language;

import tango.core.Array;

import gubg.parser.language;
import gubg.parser.tokenSequence;
import gubg.parser.token;
import gubg.puts;

// ulbu itself
class ULBULanguage: Language
{
    static this()
    {
        mSymbols = ["+", "-", ":", "{", "}", "(", ")", ";", ".", "_", "*"];

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

// the config language
class ConfigLanguage: Language
{
    static this()
    {
        mSymbols = ["=", "."];

 	foreach (symb; mSymbols)
	    mIsSymbol[symb] = true;

        mSortedSymbols = mSymbols.dup;
 	sort(mSortedSymbols, delegate bool(char[] lhs, char[] rhs){return lhs.length > rhs.length;});

        mKeywords = ["location"];

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
