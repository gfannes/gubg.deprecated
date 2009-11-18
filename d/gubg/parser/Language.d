module gubg.parser.Languange;

import tango.core.Array;

interface Language
{
    static char[][] symbols();
    static char[][] sortedSymbols(); // From long to short
    static bool isSymbol(char[] str);

    static char[][] keywords();
    static bool isKeyword(char[] str);
}

class DLanguage: Language
{
    static this()
    {
        mSymbols = ["/", "/=", ".", "..", "...", "&", "&=", "&&", "|", "|=", "||", "-", "-=", "--", "+", "+=", "++", "<", "<=", "<<", "<<=", "<>", "<>=", ">", ">=", ">>=", ">>>=", ">>", ">>>", "!", "!=", "!==", "!<>", "!<>=", "!<", "!<=", "!>", "!>=", "!~", "(", ")", "[", "]", "{", "}", "?", ",", ";", ":", "$", "=", "==", "===", "*", "*=", "%", "%=", "^", "^=", "~", "~=", "~~"];

 	foreach (symb; mSymbols)
	    mIsSymbol[symb] = true;

        mSortedSymbols = mSymbols.dup;
 	sort(mSortedSymbols, delegate bool(char[] lhs, char[] rhs){return lhs.length > rhs.length;});

	mKeywords = ["abstract", "alias", "align", "asm", "assert", "auto", "body", "bool", "break", "byte", "case", "cast", "catch", "cdouble", "cent", "cfloat", "char", "class", "const", "continue", "creal", "dchar", "debug", "default", "delegate", "delete", "deprecated", "do", "double", "else", "enum", "export", "extern", "false", "final", "finally", "float", "for", "foreach", "foreach_reverse", "function", "goto", "idouble", "if", "ifloat", "import", "in", "inout", "ref", "int", "interface", "invariant", "ireal", "is", "lazy", "long", "mixin", "module", "new", "null", "out", "override", "package", "pragma", "private", "protected", "public", "real", "return", "scope", "short", "static", "struct", "super", "switch", "synchronized", "template", "this", "throw", "true", "try", "typedef", "typeid", "typeof", "ubyte", "ucent", "uint", "ulong", "union", "unittest", "ushort", "version", "void", "volatile", "wchar", "while", "with"];
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

version (UnitTest)
{
    import gubg.Puts;

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
