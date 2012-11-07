#include "gubg/testing/Testing.hpp"
#include "gubg/SmartRange.hpp"
#include "gubg/l.hpp"
#include <string>
#include <list>
using namespace gubg;
using namespace std;

namespace 
{
    typedef SmartRange<string> SR;
}

int main()
{
    TEST_TAG(SmartRange);

    SR sr;
    {
        SR sr2("abc");
        SR sr3(sr2);
        for (auto ch: sr2)
            L(ch);
        sr = sr2;
    }
    for (auto ch: sr)
        L(ch);

    {
        auto sp = sr;
        TEST_EQ(3, sr.size());
        TEST_FALSE(sr.empty());
        ++sr.front();
        ++sr[1];
        ++sr.back();
        sr.popFront();
        TEST_EQ(2, sr.size());
        sr.popFront();
        sr.popFront();
        TEST_TRUE(sr.empty());
        sr = sp;
    }

    TEST_EQ(3, sr.size());
    for (auto ch: sr)
        L(ch);

    {
        auto p = sr.data();
        p[2] = '\0';
        L("p: " << p);

        auto ssr = sr.subrange(1, 1);
        ssr.front() = 'B';
        L("p: " << p << "|" << ssr.data());
    }

    sr = "abc";
    TEST_EQ(1, sr.find(sr.subrange(1, 1)));
    TEST_EQ(1, sr.find("b"));
    TEST_EQ(1, sr.find("b", 1));
    TEST_EQ(SR::npos, sr.find("b", 2));
    TEST_EQ(SR::npos, sr.find("B"));

#if 0
    {
        TEST_TAG(breakdown);
        list<SR> tokens;
        SR sr("a||b|ccc");
        struct Splitter
        {
            bool operator()(char p, char c)
            {
                return p != c;
            }
        };
        sr.breakdown(tokens, Splitter());
        for (auto token: tokens)
            L(string(token.data(), token.size()));
    }
#endif

    return 0;
}
