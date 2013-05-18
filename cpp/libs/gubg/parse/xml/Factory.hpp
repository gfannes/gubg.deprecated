#ifndef HEADER_gubg_parse_xml_Factory_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_xml_Factory_hpp_ALREADY_INCLUDED

#include "gubg/parse/xml/Codes.hpp"
#include "gubg/Strange.hpp"

#define GUBG_MODULE_ "xml::Factory"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace xml
    {
        template <typename Receiver>
            class Factory_crtp
            {
                public:
                    ReturnCode process(const std::string &str)
                    {
                        MSS_BEGIN(ReturnCode, str);

                        MSS(!str.empty(), XMLEmpty);
                        str_ = str;
                        while (str_.popChar('\n') || str_.popCharBack('\n')){}

                        gubg::Strange tag, attr;
                        Flags flags;
                        while (!str_.empty())
                        {
                            Strange text;
                            MSS(str_.popTo(text, '<'));
                            if (!text.empty())
                            {
                                L(STREAM(text));
                            }
                            MSS(readTag_(tag, attr, flags));
                            L(STREAM(tag, attr, flags));
                        }

                        MSS_END();
                    }
                private:
                    typedef unsigned int Flags;
                    static const Flags None = 0;
                    static const Flags Open = 1;
                    static const Flags Close = 2;
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                    ReturnCode readTag_(gubg::Strange &tag, gubg::Strange &attr, Flags &flags)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(str_.popChar('<'));
                        gubg::Strange all;
                        MSS(str_.popUntil(all, '>'));

                        if (all.popChar('/'))
                        {
                            flags = Close;
                            all.popAll(tag);
                        }
                        else
                        {
                            flags = Open;
                            if (!all.popUntil(tag, ' '))
                            {
                                //no attributes
                                all.popAll(tag);
                                if (tag.popCharBack('/'))
                                    flags |= Close;
                            }
                            else
                            {
                                //attributes
                                all.popAll(attr);
                                if (attr.popCharBack('/'))
                                    flags |= Close;
                            }
                        }
                        MSS_END();
                    }

                    Strange str_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
