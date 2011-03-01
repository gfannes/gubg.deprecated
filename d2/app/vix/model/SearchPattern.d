module vix.model.SearchPattern;

import vix.Exit;

import gubg.Format;

//There was instability in std.regex for D2.052 which is why we for now fall back to using regexp
version = UseRegExp;
version (UseRegex)
    import std.regex;
version (UseRegExp)
    import std.regexp;
import std.utf;
import std.algorithm;
import std.string;

import std.stdio;

class SearchPattern
{
    enum Case {Sensitive, Insensitive};
    this(string pattern, Case cas = Case.Sensitive)
    {
        pattern_ = pattern;
        try
        {
            switch (cas)
            {
                case Case.Sensitive:
                    auto specialChars = ['$', '.', '+', '*', '?', '\\'];
                    foreach (ch; pattern_)
                    {
                        if (canFind(specialChars, ch))
                        {
                            regexpPattern_ = RegExp(pattern_);
                            break;
                        }
                    }
                    break;
                case Case.Insensitive:
                    regexpPattern_ = RegExp(pattern_, "i");
                    break;
            }
        }
        catch (std.regexp.RegExpException){reportError(Format.immediate("Invalid regular expression: %s", pattern_));}
    }
    string getPattern() const {return pattern_;}

    bool matches(void[] data)
    {
        if (!isSimplePattern_)
        {
            reportError("For now, I can only match simple patterns again a void[]");
            return false;
        }
        if (-1 == std.string.indexOf(cast(string)data, pattern_))
            return false;
        return true;
    }
    bool matches(string str)
    {
        if (isSimplePattern_)
            return matches(cast(void[])str);
        return regexpPattern_.test(str);
    }

    private:
    bool isSimplePattern_() const
    {
        return regexpPattern_ is null;
    }
    string pattern_;
    RegExp regexpPattern_;
}
