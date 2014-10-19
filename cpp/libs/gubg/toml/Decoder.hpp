#ifndef HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED

#include "gubg/toml/Codes.hpp"
#include "gubg/Strange.hpp"
#include "gubg/OnlyOnce.hpp"
#include <string>
#include <list>
#include <stack>
#include <cassert>

#define GUBG_MODULE_ "Decoder"
#include "gubg/log/begin.hpp"
namespace gubg { namespace toml { 

    namespace details { 
        bool trimWhitespaceFront(Strange &r)
        {
            bool ret = false;
            while (r.popCharIf(' ') || r.popCharIf('\t'))
                ret = true;
            return ret;
        }
        bool trimWhitespaceBack(Strange &r)
        {
            bool ret = false;
            while (r.popCharBackIf(' ') || r.popCharBackIf('\t'))
                ret = true;
            return ret;
        }
        bool trimWhitespace(Strange &r)
        {
            auto f = trimWhitespaceFront(r);
            auto b = trimWhitespaceBack(r);
            return f || b;
        }
    } 

    template <typename Receiver>
        class Decoder_crtp
        {
            public:
                typedef Decoder_crtp<Receiver> Self;

                ReturnCode decode(const std::string &msg)
                {
                    MSS_BEGIN(ReturnCode);
                    range_ = msg;
                    path_.clear();
                    state_ = Idle;
                    assert(path_.empty());
                    while (!range_.empty())
                    {
                        if (details::trimWhitespaceFront(range_))
                            continue;
                        switch (state_)
                        {
                            case Idle:
                                if (false) {}
                                else if (readKey_())
                                {
                                    L("Read key: " << key_);
                                    state_ = Key;
                                }
                                else if (readPath_())
                                {
                                    L("Read path: " << path_);
                                    state_ = Path;
                                }
                                else
                                {
                                    MSS_L(KeyOrPathExpected);
                                }
                                break;
                            case Path:
                                MSS(range_.popCharIf('\n'), NewlineExpected);
                                state_ = Idle;
                                break;
                            case Key:
                                if (false) {}
                                else if (range_.popDecimal(int_))
                                {
                                    L("Read number: " << int_);
                                    state_ = Idle;
                                }
                                else if (readString_())
                                {
                                    L("Read string: " << str_);
                                    state_ = Idle;
                                }
                                else
                                {
                                    MSS_L(ValueExpected);
                                }
                                break;
                        }
                    }
                    MSS_END();
                }

            private:
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                bool readKey_()
                {
                    if (!range_.popUntil(key_, '='))
                        return false;
                    details::trimWhitespaceBack(key_);
                    return true;
                }
                bool readPath_()
                {
                    auto sp = range_;
                    if (!range_.popCharIf('['))
                        return false;
                    if (!range_.popUntil(path_, ']'))
                    {
                        range_ = sp;
                        return false;
                    }
                    details::trimWhitespace(path_);
                    return true;
                }
                bool readString_()
                {
                    //Keep the multiline versions first
                    return readMultiLineEscaped_() || readMultiLineUnescaped_() || readSingleLineEscaped_() || readSingleLineUnescaped_();
                }
                bool readMultiLineUnescaped_()
                {
                    auto sp = range_;
                    if (!range_.popString("'''"))
                        //This is not an unescaped multiline string
                        return false;
                    return range_.popUntil(str_, "'''");
                }
                bool readMultiLineEscaped_()
                {
                    static const std::string marker = "\"\"\"";
                    auto sp = range_;
                    if (!range_.popString(marker))
                        //This is not an unescaped multiline string
                        return false;
                    while (true)
                    {
                        if (!range_.popTo(str_, marker))
                        {
                            range_ = sp;
                            return false;
                        }
                        if (str_.empty())
                            break;
                        if (str_.back() != '\\')
                            //We found a """ sequence which is not preceded by a '\': we found the end of the string
                            break;
                        //We found a """ sequence which _is_ preceded by a '\': we need continue looking for the end
                        range_.popFront();
                    }
                    //Compute the full string content
                    str_ = sp; str_.popFront();
                    str_.diffTo(range_);
                    //Removed the trailing """ from range_
                    range_.popString(marker);
                    //TODO: unescaping is not OK and does not deal with all corner cases yet
                    if (!str_.contains('\\'))
                        //str_ has no escaped characters, we are done
                        return true;
                    //We have to unescape str_
                    std::string tmp;
                    Strange part;
                    while (str_.popUntil(part, '\\'))
                    {
                        tmp.append(part.str());
                        char ch;
                        if (!str_.popChar(ch))
                        {
                            range_ = sp;
                            return false;
                        }
                        switch (ch)
                        {
                            case 'b': tmp.push_back('\b'); break;
                            case 't': tmp.push_back('\t'); break;
                            case 'n': tmp.push_back('\n'); break;
                            case 'f': tmp.push_back('\f'); break;
                            case 'r': tmp.push_back('\r'); break;
                            case '\"': tmp.push_back('\"'); break;
                            case '\\': tmp.push_back('\\'); break;
                            case '/': tmp.push_back('/'); break;
                            default: tmp.push_back(ch); break;
                        }
                    }
                    plainStrings_.push_back(std::move(tmp));
                    str_ = plainStrings_.back();
                    return true;
                }
                bool readSingleLineUnescaped_()
                {
                    auto sp = range_;
                    if (!range_.popCharIf('\''))
                        //This is not an unescaped string
                        return false;
                    return range_.popUntil(str_, '\'');
                }
                bool readSingleLineEscaped_()
                {
                    auto sp = range_;
                    if (!range_.popCharIf('"'))
                        //This is not an escaped string
                        return false;
                    while (true)
                    {
                        if (!range_.popTo(str_, '"'))
                        {
                            range_ = sp;
                            return false;
                        }
                        if (str_.empty())
                            break;
                        if (str_.back() != '\\')
                            //We found a '"' character which is not preceded by a '\': we found the end of the string
                            break;
                        //We found a '"' character which _is_ preceded by a '\': we need continue looking for the end
                        range_.popFront();
                    }
                    //Compute the full string content
                    str_ = sp; str_.popFront();
                    str_.diffTo(range_);
                    //Removed the trailing '"' from range_
                    range_.popFront();
                    if (!str_.contains('\\'))
                        //str_ has no escaped characters, we are done
                        return true;
                    //We have to unescape str_
                    std::string tmp;
                    Strange part;
                    while (str_.popUntil(part, '\\'))
                    {
                        tmp.append(part.str());
                        char ch;
                        if (!str_.popChar(ch))
                        {
                            range_ = sp;
                            return false;
                        }
                        switch (ch)
                        {
                            case 'b': tmp.push_back('\b'); break;
                            case 't': tmp.push_back('\t'); break;
                            case 'n': tmp.push_back('\n'); break;
                            case 'f': tmp.push_back('\f'); break;
                            case 'r': tmp.push_back('\r'); break;
                            case '\"': tmp.push_back('\"'); break;
                            case '\\': tmp.push_back('\\'); break;
                            case '/': tmp.push_back('/'); break;
                            default: tmp.push_back(ch); break;
                        }
                    }
                    plainStrings_.push_back(std::move(tmp));
                    str_ = plainStrings_.back();
                    return true;
                }
                bool readChar_(char wanted)
                {
                    if (range_.front() != wanted)
                        return false;
                    range_.popFront();
                    return true;
                }
                static bool isWhitespace_(char ch)
                {
                    switch (ch)
                    {
                        case ' ':
                        case '\t':
                            return true;
                    }
                    return false;
                }
                static bool isKeyChar_(char ch, bool first)
                {
                    if ('a' <= ch && ch <= 'z')
                        return true;
                    if ('A' <= ch && ch <= 'Z')
                        return true;
                    switch (ch)
                    {
                        case '+':
                            return true;
                    }
                    if (first)
                    {
                        if ('0' <= ch && ch <= '9')
                            return true;
                    }
                    return false;
                }
                static bool isIntegerChar_(char ch, bool first)
                {
                    if ('0' <= ch && ch <= '9')
                        return true;
                    if (first)
                    {
                        switch (ch)
                        {
                            case '+':
                            case '-':
                                return true;
                        }
                    }
                    return false;
                }

                Strange range_;
                Strange path_;
                Strange key_;
                long int_;
                Strange str_;
                //We keep all unescaped strings in a list to make sure the Stranges we give out have the same lifetime as self
                typedef std::list<std::string> PlainStrings;
                PlainStrings plainStrings_;
                enum State {Idle, Key, Path,};
                State state_;
        };

} } 
#include "gubg/log/end.hpp"

#endif
