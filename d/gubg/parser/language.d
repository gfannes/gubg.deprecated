module gubg.parser.languange;

import tango.core.Array;

interface Language
{
    static char[][] symbols();
    static char[][] sortedSymbols(); // From long to short
}

class DLanguage: Language
{
    static this()
    {
        mSymbols = ["/", "/=", ".", "..", "...", "&", "&=", "&&", "|", "|=", "||", "-", "-=", "--", "+", "+=", "++", "<", "<=", "<<", "<<=", "<>", "<>=", ">", ">=", ">>=", ">>>=", ">>", ">>>", "!", "!=", "!==", "!<>", "!<>=", "!<", "!<=", "!>", "!>=", "!~", "(", ")", "[", "]", "{", "}", "?", ",", ";", ":", "$", "=", "==", "===", "*", "*=", "%", "%=", "^", "^=", "~", "~=", "~~"];

        mSortedSymbols = mSymbols.dup;
 	sort(mSortedSymbols, delegate bool(char[] lhs, char[] rhs){return lhs.length > rhs.length;});
    }

    static char[][] symbols(){return mSymbols;}
    static char[][] sortedSymbols(){return mSortedSymbols;}

private:
    static char[][] mSymbols;
    static char[][] mSortedSymbols;
}

class DITFLanguage: Language
{
    static this()
    {
        mSymbols = ["+", "-", ":", "{", "}", "(", ")", ";", "."];

        mSortedSymbols = mSymbols.dup;
 	sort(mSortedSymbols, delegate bool(char[] lhs, char[] rhs){return lhs.length > rhs.length;});
    }

    static char[][] symbols(){return mSymbols;}
    static char[][] sortedSymbols(){return mSortedSymbols;}

private:
    static char[][] mSymbols;
    static char[][] mSortedSymbols;
}

version (Test)
{
    import gubg.puts;

    class A(T)
    {
        void print()
            {
                foreach (symb; T.symbols)
                    puts("{}", symb);
            }
    }

    void main()
    {
        auto a = new A!(DLanguage);
        a.print;
    }
}
