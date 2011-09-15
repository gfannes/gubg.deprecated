#ifndef regex_h
#define regex_h

#include <tre/regex.h>
#include <string>
#include <iostream>

using namespace std;

class Regex
{
public:
    Regex(string regex, bool caseSensitive = true):
        mValid(false),
        mCS(caseSensitive)
        {
            setRegex(regex);
        }
    ~Regex()
        {
            if (mValid)
                regfree(&mRegex);
        }
    
    bool setRegex(string regex)
        {
            mStrRegex = regex;
            if (mValid)
                regfree(&mRegex);
            int cflags = REG_EXTENDED;
            if (!mCS)
                cflags |= REG_ICASE;
            return mValid = (regncomp(&mRegex, mStrRegex.c_str(), mStrRegex.size(), cflags) == 0);
        }
    
    bool match(string &str)
        {
            if (!mValid)
            {
                cerr << "Regex is not valid, I cannot match." << endl;
                return false;
            }
            return regnexec(&mRegex, str.c_str(), str.size(), 0, NULL, 0x00) == 0;
        }

    string toString(){return mStrRegex;}
private:
    string mStrRegex;
    bool mValid;
    bool mCS;
    regex_t mRegex;
};

#endif
