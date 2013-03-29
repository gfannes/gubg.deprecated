#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/Platform.hpp"
#include <sstream>
#include <cstring>
#include <cassert>
using namespace std;

//#define GUBG_MODULE "Lexer"
#include "gubg/log/begin.hpp"

#ifndef GUBG_LINUX
namespace std
{
    bool isblank(char ch)
    {
        return ' ' == ch || '\t' == ch;
    }
}
#endif

namespace 
{
    bool isIdentifier_(char ch)
    {
        return isalpha(ch) || '_' == ch || isdigit(ch);
    }
    template <typename It>
        size_t backslashNewline_(It b, It e)
        {
            size_t ret = 0;
            while (b != e)
            {
                if ('\\' != *b++)
                    return ret;
                switch (*b++)
                {
                    case '\n': ret += 2; break;
                    case '\r':
                               if (b == e || '\n' != *b++)
                                   return ret;
                               ret += 3;
                               break;
                    default:
                               return ret;
                }
            }
            return ret;
        }
    struct Prev_raii
    {
        char &prev;
        const char ch;
        Prev_raii(char &p, char c):
            prev(p), ch(c){}
        ~Prev_raii(){prev = ch;}
    };
    template <typename It>
        char nextChar_(It b, It e)
        {
            b += backslashNewline_(b, e);
            if (b == e)
                return '\0';
            return *b;
        }
}
namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            namespace pp
            {
                LexerState::LexerState()
                {
                    clear();
                }
                void LexerState::clear()
                {
                    state_ = Idle;
                    escaped_ = false;
                    macro_ = false;
                    bsnl_ = 0;
                    pch_ = '\0';
                    line_ = 1;
                    prevType_ = type_ = Type::Unknown;
                }

                bool LexerState::findToken(It s, It m, It e)
                {
                    if (state_ == Error)
                        return false;
                    if (state_ == Detected)
                        //You have to call getToken() to get out of the Detected state
                        return false;

                    const auto lch = *(m-1);
                    //Keep track of the line number
                    if (lch == '\n')
                        ++line_;

                    if (bsnl_ > 0)
                    {
                        --bsnl_;
                        return false;
                    }
                    if ((bsnl_ = backslashNewline_(m-1, e)) > 0)
                    {
                        --bsnl_;
                        return false;
                    }
                    Prev_raii prev_raii(pch_, lch);

                    range_.limit(s, m);

                    if (state_ == Idle)
                    {
                        const auto sch = *s;
                        //Single-character stuff
                        switch (sch)
                        {
                            case '\n': macro_ = false;//Any macro is ended with a newline, at least after we deal with backslash-newlines
                                       return detectedA_(Type::LineFeed);
                            case '\r': return detectedA_(Type::CarriageReturn);
                                       //These tokens cannot be ended by themselves, we can dirrectly return
                            case '#': if (macro_)
                                          //Inside a macro, "#" and "##" are legitimate symbols
                                          return detectedA_(Type::Symbol);
                                      macro_ = true;
                                      if (m == e)
                                          return error("Unfinished macro");
                                      if (isIdentifier_(nextChar_(m, e)))
                                          return detectedA_(Type::MacroHash);
                                      return detectingA_(Type::MacroHash);
                            case '"': return detectingA_(Type::String);
                            case '\'': return detectingA_(Type::Character);
                            case '/':
                                       if (m == e)
                                           return detectedA_(Type::Symbol);
                                       switch (nextChar_(m, e))
                                       {
                                           case '/': return detectingA_(Type::LineComment);
                                           case '*': return detectingA_(Type::BlockComment);
                                       }
                                       return detectedA_(Type::Symbol);
                            case '\\': return detectedA_(Type::StandaloneBackslashNewline);
                        }
                        static const char symbols[] = ":.;,(){}[]<>=-+*|&^%!?~$";
                        if (memchr(symbols, sch, sizeof(symbols)))
                            return detectedA_(Type::Symbol);
                        //These tokens might be ended immediately, we do not return, but will also execute the Detecting part
                        if (isIdentifier_(sch))
                        {
                            if (prevType_ == Type::MacroHash)
                                detectingA_(Type::Macro);
                            else
                                detectingA_(Type::Identifier);
                        }
                        else if (isblank(sch))
                            detectingA_(Type::Blanks);
                        else
                        {
                            ostringstream oss;
                            oss << "Unknown character encountered in Idle state on line " << line_ << ": \"" << sch << "\" (" << (int)sch << ")";
                            return error(oss.str());
                        }
                    }

                    if (state_ == Detecting)
                    {
                        if (m == e)
                        {
                            //This means we are at the end of file
                            switch (type_)
                            {
                                case Type::Unknown: assert(false); break;
                                case Type::MacroHash: return error("Incomplete macro");
                                case Type::Macro:
                                case Type::Identifier:
                                case Type::Blanks:
                                case Type::LineComment:
                                                      return detected_();
                                case Type::String: return error("String is not closed");
                                case Type::Character: return error("Character is not closed");
                                case Type::BlockComment: return error("BlockComment is not closed");
                            }
                            return error("Unexpected end of file");
                        }
                        const auto nch = nextChar_(m, e);
                        switch (type_)
                        {
                            case Type::Unknown: assert(false); break;
                            case Type::String:
                                                switch (lch)
                                                {
                                                    case '\"': if (!escaped_) return detected_(); break;
                                                    case '\\': escaped_ = !escaped_; return false; break;
                                                }
                                                escaped_ = false;
                                                break;
                            case Type::Character:
                                                switch (lch)
                                                {
                                                    case '\'': if (!escaped_) return detected_(); break;
                                                    case '\\': escaped_ = !escaped_; return false; break;
                                                }
                                                escaped_ = false;
                                                break;
                            case Type::MacroHash: if (isalpha(nch)) return detected_(); break;
                            case Type::Macro: if (!isalpha(nch)) return detected_(); break;
                            case Type::Identifier: if (!isIdentifier_(nch)) return detected_(); break;
                            case Type::Blanks: if (!isblank(nch)) return detected_(); break;
                            case Type::LineComment: if (nch == '\n') return detected_(); break;
                            case Type::BlockComment: if (lch == '/' && pch_ == '*' && range_.size() >= 4) return detected_(); break;
                        }
                        //We have to wait some more
                        return false;
                    }
                    assert(false);
                    return false;
                }
                ReturnCode LexerState::getToken(Token &token)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS(state_ == Detected);
                    token.type = type_;
                    prevType_ = type_;
                    type_ = Type::Unknown;
                    token.range = range_;
                    state_ = Idle;
                    MSS_END();
                }

                bool LexerState::detectingA_(Type t)
                {
                    type_ = t;
                    state_ = Detecting;
                    return false;
                }
                bool LexerState::detectedA_(Type t)
                {
                    printToken_('<', '>');
                    type_ = t;
                    state_ = Detected;
                    return true;
                }
                bool LexerState::detected_()
                {
                    printToken_('(', ')');
                    state_ = Detected;
                    return true;
                }
                bool LexerState::error(const string &msg)
                {
                    LL("ERROR \"" << msg << "\" on line " << line_);
                    state_ = Error;
                    errorMsg_ = msg;
                    return false;
                }

                void LexerState::printToken_(char cho, char chc)
                {
#ifdef L_DEBUG
                    cout << cho << string(range_.begin(), range_.end()) << chc;
#endif
                }
                void LexerState::printToken_(char ch)
                {
                    printToken_(ch, ch);
                }
            };
        }
    }
}
