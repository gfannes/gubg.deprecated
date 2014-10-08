#ifndef HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED

#include "gubg/toml/Codes.hpp"
#include "gubg/Strange.hpp"
#include <string>
#include <stack>
#include <cassert>

#define GUBG_MODULE_ "Decoder"
#include "gubg/log/begin.hpp"
namespace gubg { namespace toml { 

	template <typename Receiver>
		class Decoder_crtp
		{
			public:
                typedef Decoder_crtp<Receiver> Self;

				ReturnCode decode(const std::string &msg)
				{
					MSS_BEGIN(ReturnCode);
					range_ = msg;
					path_clear();
                    state_ = Idle;
					assert(path_.empty());
                    while (!range_.empty())
                    {
                        if (trimWhitespace_())
                            continue;
                        switch (state_)
                        {
                            case Idle:
                                if (false) {}
                                else if (readKey_())
                                {
                                    L("Read key: " << str_(key_));
                                    state_ = Key;
                                }
                                else if (readPath_())
                                {
                                    L("Read path: " << str_(path_));
                                    state_ = Path;
                                }
                                else
                                {
                                    MSS_L(KeyOrPathExpected);
                                }
                                break;
                            case Key:
                                MSS(readChar_('='), EqualSignExpected);
                                state_ = EqualSign;
                                break;
                            case Path:
                                MSS(readChar_('\n'), NewlineExpected);
                                state_ = Idle;
                                break;
                            case EqualSign:
                                if (false) {}
                                else if (readInterger_())
                                {
                                    L("Read number: " << str_(int_));
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
				bool trimWhitespace_()
				{
					return range_.popCharIf(' ') || range_.popCharIf('\t')
				}
                bool readKey_()
                {
					key_ = range_;
					gubg::OnlyOnce first;
                    for (; !range_.empty() && isKeyChar_(range_.front(), first()); range_.popFront())
                    {
                    }
					key_.subtract(range_);
					return !key_.empty();
                }
                bool readPath_()
                {
                    if (range_.front() != '[')
                       return false;
                    range_.popFront();
                    auto b = range_.begin();
                    for (; !range_.empty(); range_.popFront())
                    {
                        if (range_.front() == ']')
                        {
                            path_ = Range(b, range_.begin());
                            range_.popFront();
                            return true;
                        }
                    }
                    return false;
                }
                bool readChar_(char wanted)
                {
                    if (range_.front() != wanted)
                        return false;
                    range_.popFront();
                    return true;
                }
                bool readInterger_()
                {
                    auto b = range_.begin();
                    for (; !range_.empty() && isIntegerChar_(range_.front(), b == range_.begin()); range_.popFront())
                    {
                    }
                    auto e = range_.begin();
                    if (b == e)
                        return false;
                    int_ = Range(b, range_.begin());
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
                static std::string str_(const Range &r)
                {
                    return std::string(r.begin(), r.end());
                }

				Strange range_;
				Strange path_;
                Strange key_;
                Strange int_;
                enum State {Idle, Key, EqualSign, Path,};
                State state_;
        };

} } 
#include "gubg/log/end.hpp"

#endif
