module rinle.common.helper;

// Some helper functions
// Check if token is found at start of str
bool startWith(char[] token, char[] str)
{
    if ((token.length <= str.length) && (token == str[0 .. token.length]))
        return true;
    return false;
}

// Return first occurrence of token
// Returns the index of first charachter of token
int findFirstF(char[] token, char[] str)
{
    for (int i=0; i<str.length; ++i)
    {
        if (startWith(token, str[i .. $]))
            return i;
    }
    return -1;
}

// Returns the index of last character of token
int findFirstL(char[] token, char[] str)
{
    int ix = findFirstF(token, str);
    if (ix >= 0)
        ix += token.length;
    return ix;
}

// Returns the index of first character of tokenA or tokenB
int findFirstF(char[] tokenA, char[] tokenB, char[] str)
{
    for (int i=0; i<str.length; ++i)
    {
        if (startWith(tokenA, str[i .. $]))
            return i;
        if (startWith(tokenB, str[i .. $]))
            return i;
    }
    return -1;
}

// Returns the index of first character of tokens
int findFirstF(out int ixToken, char[][] tokens, char[] str)
{
    int ix = -1;
    ixToken = -1;
    foreach (ixT, token; tokens)
    {
	int ixTmp = findFirstF(token, str);
	if (ixTmp >= 0)
	{
	    if (ix < 0)
	    {
		ix = ixTmp;
		ixToken = ixT;
	    } else if (ixTmp < ix)
	    {
		ix = ixTmp;
		ixToken = ixT;
	    }
	}
    }
    return ix;
}
