module rinle.d.parser.tokenSequence;

import tango.text.Util;

import gubg.list;
import rinle.d.parser.token;
import rinle.common.helper;
import gubg.puts;

class TokenSequence: List!(Token)
{
    this (char[] sourceCode)
    {
        // Split up in sequence of Comment, String and Unknown
        splitCommentsStrings(sourceCode);

        // Split Unknowns into Unknowns based on whitespaces and EOL
        splitWhiteSpaceEOL();

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
        Token[] tokens = [cast(Token)new String(), cast(Token)new Comment()];
	    
        char[] pre, post, rest = sourceCode;
        Token token;
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
                        add(new Token(pre));
                    add(token);
                    ix = 0;
                    rest = post;
                    break;
                }
            }
        }
        if (rest.length > 0)
            add(new Token(rest));
    }

    void splitWhiteSpaceEOL()
    {
        auto list = new List!(Token);
        handover(list);
        foreach (token; list)
            if (token.known)
                add(token);
            else
                foreach (line; splitLines(token.str))
                    if (line.length > 0)
                        foreach (str; split(line, " "))
                            if (str.length > 0)
                                foreach (str2; split(str, "\t"))
                                    if (str2.length > 0)
                                        add(new Token(str2));
    }

    void processSpecialTokenSequences()
    {
    }

    void splitTokens()
    {
	auto list = new List!(Token);
	handover(list);

	Symbol symbol = new Symbol;

	foreach (token; list)
	    if (token.known)
		add(token);
	    else
	    {
		char[] pre, post, rest = token.str;
		Symbol s;
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
			    add(new Identifier(pre));
			add(s);
			ix = 0;
			rest = post;
		    }
		}
		if (rest.length > 0)
		    add(new Identifier(rest));
	    }
    }

    void replaceSpecialTokens()
    {
    }

    void joinTokens()
    {
	auto list = new List!(Token);
	handover(list);

	bool skipNext = false;
	list.each(delegate bool(Token token, Token* next)
		  {
		      if (skipNext)
			  skipNext = false;
		      else
		      {
			  if (next !is null && token.isSymbol("!") && next.isSymbol("is"))
			  {
			      add(new Symbol("!is"));
			      skipNext = true;
			  } else
			      add(token);
		      }
		      return true;
		  });
    }
}

version (Test)
{
    import gubg.file;

    void main()
    {
	char[] sourceCode;
	loadFile(sourceCode, "tokenSequence.d");
	TokenSequence ts = new TokenSequence(sourceCode);

	foreach (token; ts)
	    puts("token({}) = \"{}\"", token.known, token.str);
	puts("Nr tokens = {}", ts.length);
    }
}
