#include "gubg/parse/xml/Coding.hpp"
#include "gubg/Strange.hpp"
#include <cctype>
using namespace std;

#define GUBG_MODULE "XMLCoding"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace xml
    {
        ReturnCode encode(string &enc, const string &raw)
        {
            MSS_BEGIN(ReturnCode);

            //We compute the size after encoding to avoid repetitive reallocations
            size_t encSize = raw.size();
            for (auto ch: raw)
            {
                switch (ch)
                {
                    case '"': encSize += 5; break;
                    case '<': encSize += 3; break;
                    case '>': encSize += 3; break;
                    case '&': encSize += 4; break;
                    default:
                              MSS(std::isprint(ch) != 0 || ch == '\n' || ch == '\r' || ch == '\t', NonPrintable);
                              break;
                }
            }

            if (encSize == raw.size())
                //No escaping is necessary, the raw string _is_ the encoded string
                enc = raw;
            else
            {
                enc.resize(encSize);
                auto dst = enc.begin();
                for (auto ch: raw)
                {
                    assert(dst != enc.end());
                    switch (ch)
                    {
                        case '"': *dst++ = '&'; *dst++ = 'q'; *dst++ = 'u'; *dst++ = 'o'; *dst++ = 't'; *dst++ = ';'; break;
                        case '<': *dst++ = '&'; *dst++ = 'l'; *dst++ = 't'; *dst++ = ';'; break;
                        case '>': *dst++ = '&'; *dst++ = 'g'; *dst++ = 't'; *dst++ = ';'; break;
                        case '&': *dst++ = '&'; *dst++ = 'a'; *dst++ = 'm'; *dst++ = 'p'; *dst++ = ';'; break;
                        default:  *dst++ = ch; break;
                    }
                }
                assert(dst == enc.end());
            }

            MSS_END();
        }

        ReturnCode decode(string &raw, const string &enc)
        {
            MSS_BEGIN(ReturnCode);

            //We compute the size after decoding
            size_t rawSize = enc.size();
            {
                gubg::Strange strange(enc);
                char ch;
                while (!strange.empty())
                {
                    if (strange.popCharIf('&'))
                    {
                        if (false) {}
                        else if (strange.popString("quot;"))
                        {
                            MSS(rawSize >= 5);
                            rawSize -= 5;
                        }
                        else if (strange.popString("lt;"))
                        {
                            MSS(rawSize >= 3);
                            rawSize -= 3;
                        }
                        else if (strange.popString("gt;"))
                        {
                            MSS(rawSize >= 3);
                            rawSize -= 3;
                        }
                        else if (strange.popString("amp;"))
                        {
                            MSS(rawSize >= 4);
                            rawSize -= 4;
                        }
                        else if (strange.popString("apos;"))
                        {
                            MSS(rawSize >= 5);
                            rawSize -= 5;
                        }
                        else if (strange.popString("#xa;") || strange.popString("#xA;") || strange.popString("#xd;") || strange.popString("#xD;"))
                        {
                            MSS(rawSize >= 4);
                            rawSize -= 4;
                        }
                        else
                            MSS_L(UnknownEscape);
                    }
                    else
                    {
                        const auto ret = strange.popChar(ch);
                        assert(ret);
                        MSS(ch != '"' && ch != '<' && ch != '>' && ch != '&', UnexpectedRawChar);
                    }
                }
            }

            if (rawSize == enc.size())
                //The encoded string _is_ the raw string
                raw = enc;
            else
            {
                raw.resize(rawSize);
                auto dst = raw.begin();
                char ch;
                gubg::Strange strange(enc);
                while (!strange.empty())
                {
                    if (strange.popCharIf('&'))
                    {
                        if (false) {}
                        else if (strange.popString("quot;"))
                            *dst++ = '"';
                        else if (strange.popString("lt;"))
                            *dst++ = '<';
                        else if (strange.popString("gt;"))
                            *dst++ = '>';
                        else if (strange.popString("amp;"))
                            *dst++ = '&';
                        else if (strange.popString("apos;"))
                            *dst++ = '\'';
                        else if (strange.popString("#xa;") || strange.popString("#xA;"))
                            *dst++ = '\n';
                        else if (strange.popString("#xd;") || strange.popString("#xD;"))
                            *dst++ = '\r';
                        else
                        {
                            //Should have failed above
                            assert(false);
                        }
                    }
                    else
                    {
                        strange.popChar(ch);
                        *dst++ = ch;
                    }
                }
                assert(dst == raw.end());
            }

            MSS_END();
        }
    }
}
#include "gubg/log/end.hpp"
