//#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/parse/cpp/Parser.hpp"
#include "gubg/parse/BreakUp.hpp"
#include "gubg/file/Filesystem.hpp"
#include <cassert>
#include <cctype>
#include <sstream>
using namespace gubg::parse::cpp;
using namespace std;

#define GUBG_MODULE "Parser"
#include "gubg/log/begin.hpp"
ReturnCode Parser::parse(const gubg::file::File &file)
{
    MSS_BEGIN(ReturnCode, parse);

    //Read file into range
    MSS(gubg::file::read(range_, file));
    LOG_M("I read " << range_.size() << " bytes");

    MSS(parseNonRecursive_());

    MSS_END();
}

namespace 
{
    bool isalpha_(char ch)
    {
        return isalpha(ch) || '_' == ch;
    }

    typedef Range::iterator It;
    struct Consumer
    {
        enum State {Error, Idle, Detecting};
        enum Type {Macro, String, Character, Alphas, Digits, Blanks, Newline, CarriageReturn, Symbol, LineComment, BlockComment};

        State state;
        Type type;
        struct R {It b; It e; size_t size() const {return e-b;}};
        R token;
        bool escaped;
        string errorMsg;

        Consumer():state(Idle), escaped(false){}

        bool operator()(It s, It m, It e)
        {
            if (state == Error)
                return false;

            token.b = s;
            token.e = m;

            if (state == Idle)
            {
                const auto sch = *s;
                //Single-character stuff
                switch (sch)
                {
                    case '\n': return detectedA(Newline);
                    case '\r': return detectedA(CarriageReturn);
                               //These tokens cannot be ended by themselves, we can dirrectly return
                    case '#': return detectingA(Macro);
                    case '"': return detectingA(String);
                    case '\'': return detectingA(Character);
                    case '/':
                               if (m == e)
                                   return detectedA(Symbol);
                               switch (*m)
                               {
                                   case '/': return detectingA(LineComment);
                                   case '*': return detectingA(BlockComment);
                               }
                               return detectedA(Symbol);
                }
                static const char symbols[] = ":.;,(){}[]<>=-+*|&^%!?~";
                if (memchr(symbols, sch, sizeof(symbols)))
                    return detectedA(Symbol);
                //These tokens might be ended immediately, we do not return, but will also execute the Detecting part
                if (isalpha_(sch))
                    detectingA(Alphas);
                else if (isblank(sch))
                    detectingA(Blanks);
                else if (isdigit(sch))
                    detectingA(Digits);
                else
                {
                    ostringstream oss;
                    oss << "Unknown character encountered in Idle state: \"" << sch << "\" (" << (int)sch << ")";
                    return error(oss.str());
                }
            }

            if (state == Detecting)
            {
                const auto lch = *(m-1);
                if (m == e)
                {
                    switch (type)
                    {
                        case Macro:
                            if (escaped)
                                return error("Macro linending is still escaped, unexpected EOF");
                        case Alphas:
                        case Blanks:
                        case Digits:
                        case LineComment:
                            return detected();
                        case String: return error("String is not closed");
                        case Character: return error("Character is not closed");
                        case BlockComment: return error("BlockComment is not closed");
                    }
                    return error("Unexpected end of file");
                }
                const auto nch = *m;
                switch (type)
                {
                    case Macro:
                        switch (nch)
                        {
                            case '\n': if (!escaped) return detected(); escaped = false; break;
                            case '\\': escaped = !escaped; return false; break;
                        }
                        break;
                    case String:
                        switch (lch)
                        {
                            case '\"': if (!escaped) return detected(); break;
                            case '\\': escaped = !escaped; return false; break;
                        }
                        escaped = false;
                        break;
                    case Character:
                        switch (lch)
                        {
                            case '\'': if (!escaped) return detected(); break;
                            case '\\': escaped = !escaped; return false; break;
                        }
                        escaped = false;
                        break;
                    case Alphas: if (!isalpha_(nch)) return detected(); break;
                    case Blanks: if (!isblank(nch)) return detected(); break;
                    case Digits: if (!isdigit(nch)) return detected(); break;
                    case LineComment: if (nch == '\n') return detected(); break;
                    case BlockComment: if (lch == '/' && *(m-2) == '*' && token.size() >= 4) return detected(); break;
                }
                //We have to wait some more
                return false;
            }
            assert(false);
            return false;
        }

        bool detectingA(Type t)
        {
            type = t;
            state = Detecting;
            return false;
        }
        bool detectedA(Type t)
        {
            printToken('<', '>');
            return true;
        }
        bool detected()
        {
            printToken('(', ')');
            state = Idle;
            return true;
        }
        void printToken(char cho, char chc)
        {
            S();L(cho << string(token.b, token.e) << chc);
        }
        void printToken(char ch)
        {
            printToken(ch, ch);
        }
        bool error(const string &msg)
        {
            L("ERROR: " << msg);
            state = Error;
            errorMsg = msg;
            return false;
        }
    };
}
ReturnCode Parser::parseNonRecursive_()
{
    MSS_BEGIN(ReturnCode);

    Consumer consumer;
    MSS(breakUp(range_.begin(), range_.end(), consumer), ParsingFailed);

    MSS_END();
}
#include "gubg/log/end.hpp"
