#ifndef HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED

#include "gubg/toml/Codes.hpp"
#include "gubg/Strange.hpp"
#include "gubg/OnlyOnce.hpp"
#include <string>
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
                            case Key:
                                MSS(range_.popCharIf('='), EqualSignExpected);
                                state_ = EqualSign;
                                break;
                            case Path:
                                MSS(range_.popCharIf('\n'), NewlineExpected);
                                state_ = Idle;
                                break;
                            case EqualSign:
                                if (false) {}
                                else if (range_.popDecimal(int_))
                                {
                                    L("Read number: " << int_);
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
                    key_ = range_;
                    gubg::OnlyOnce first;
                    for (; !range_.empty() && isKeyChar_(range_.front(), first()); range_.popFront())
                    {
                    }
                    key_.diffTo(range_);
                    return !key_.empty();
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
                enum State {Idle, Key, EqualSign, Path,};
                State state_;
        };

} } 
#include "gubg/log/end.hpp"

#endif
