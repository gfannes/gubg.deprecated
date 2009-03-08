module gubg.parser.tokenSequence;

import tango.text.Util;

import gubg.parser.token;
import gubg.parser.helper;

import gubg.list;
import gubg.puts;

class TokenSequence(Language): List!(Token!(Language))
{
    alias Token!(Language) TokenT;

    this (char[] sourceCode)
    {
        // Split up in sequence of Comment, String and Unknown
        splitCommentsStrings(sourceCode);

        // Split Unknowns into Unknowns based on whitespaces and EOL
        splitWhiteSpaceEOL(Language.isSymbol("\n"));

        // 
        processSpecialTokenSequences();

        // Split the remaining Unknowns into tokens
        splitTokens();

        replaceSpecialTokens();

	joinTokens();
    }

    bool isSymbol(char[] symb)
    {
	if (empty || !peep.isSymbol(symb))
	    return false;
	pop;
	return true;
    }

    bool isSymbol(char[][] symbs, inout char[] symb)
    {
	if (empty || !peep.isSymbol(symbs))
	    return false;
	symb = pop.str.dup;
	return true;
    }

    bool getSymbol(inout char[] symb)
    {
	if (empty || !peep.getSymbol(symb))
	    return false;
	pop;
	return true;
    }

    bool isKeyword(char[] keyw)
    {
	if (empty || !peep.isKeyword(keyw))
	    return false;
	pop;
	return true;
    }

    bool isKeyword(char[][] keyws, inout char[] keyw)
    {
	if (empty || !peep.isKeyword(keyws))
	    return false;
	keyw = pop.str.dup;
	return true;
    }

    bool getKeyword(inout char[] keyw)
    {
	if (empty || !peep.getKeyword(keyw))
	    return false;
	pop;
	return true;
    }

    bool isIdentifier(char[] ident)
    {
	if (empty || !peep.isIdentifier(ident))
	    return false;
	pop;
	return true;
    }

    bool getIdentifier(inout char[] ident)
    {
	if (empty || !peep.getIdentifier(ident))
	    return false;
	pop;
	return true;
    }

    bool getString(inout char[] str)
    {
	if (empty || !peep.getString(str))
	    return false;
	pop;
	return true;
    }

    bool getComment(inout char[] comment)
    {
	if (empty || !peep.getComment(comment))
	    return false;
	pop;
	return true;
    }

    bool create(T)(inout T res)
    {
	return (res = T.create(this)) !is null;
    }

private:
    void splitCommentsStrings(char[] sourceCode)
    {
        TokenT[] tokens = [cast(TokenT)new String!(Language)(), cast(TokenT)new Comment!(Language)()];
	    
        char[] pre, post, rest = sourceCode;
        TokenT token;
        uint ix = 0;
        while (ix < rest.length)
        {
            pre = rest[0 .. ix];
            post = rest[ix .. $];
            ++ix;
            foreach (t; tokens)
            {
                token = t.create(post);
                if (token !is null)
                {
                    if (pre.length > 0)
                        add(new TokenT(pre));
                    add(token);
                    ix = 0;
                    rest = post;
                    break;
                }
            }
        }
        if (rest.length > 0)
            add(new TokenT(rest));
    }

    void splitWhiteSpaceEOL(bool keepEOL = false)
    {
        puts("Splitting while keeping EOL = {}", keepEOL);

        auto list = new List!(TokenT);
        handover(list);
        foreach (token; list)
            if (token.known)
                add(token);
            else
            {
                if (keepEOL)
                {
                    if (token.str.length > 0)
                        foreach (str; split(token.str, " "))
                            if (str.length > 0)
                                foreach (str2; split(str, "\t"))
                                    if (str2.length > 0)
                                        add(new TokenT(str2));
                } else
                {
                    foreach (line; splitLines(token.str))
                        if (line.length > 0)
                            foreach (str; split(line, " "))
                                if (str.length > 0)
                                    foreach (str2; split(str, "\t"))
                                        if (str2.length > 0)
                                            add(new TokenT(str2));
                }
            }
    }

    void processSpecialTokenSequences()
    {
    }

    void splitTokens()
    {
	auto list = new List!(TokenT);
	handover(list);

	auto symbol = new Symbol!(Language);

	foreach (token; list)
	    if (token.known)
		add(token);
	    else
	    {
		char[] pre, post, rest = token.str;
		Symbol!(Language) s;
		uint ix = 0;
		while (ix < rest.length)
		{
		    pre = rest[0 .. ix];
		    post = rest[ix .. $];
		    ++ix;
		    s = symbol.create(post);
		    if (s !is null)
		    {
			if (pre.length > 0)
			    add(new Identifier!(Language)(pre));
			add(s);
			ix = 0;
			rest = post;
		    }
		}
		if (rest.length > 0)
		    add(new Identifier!(Language)(rest));
	    }
    }

    void replaceSpecialTokens()
    {
    }

    void joinTokens()
    {
	auto list = new List!(TokenT);
	handover(list);

	bool skipNext = false;
	list.each(delegate bool(TokenT token, TokenT* next)
		  {
		      if (skipNext)
			  skipNext = false;
		      else
		      {
			  if (next !is null &&
                              token.isSymbol("!") &&
                              (next.isKeyword("is") || next.isKeyword("in")))
			  {
			      add(new Symbol!(Language)("!" ~ next.str));
			      skipNext = true;
			  } else
			      add(token);
		      }
		      return true;
		  });
    }
}

version (UnitTest)
{
    import gubg.file;
    import gubg.parser.language;

    void main()
    {
	char[] sourceCode;
	loadFile(sourceCode, "tokenSequence.d");
	auto ts = new TokenSequence!(DLanguage)(sourceCode);

	foreach (token; ts)
	    puts("token({}) = \"{}\"", token.known, token.str);
	puts("Nr tokens = {}", ts.length);
    }
}