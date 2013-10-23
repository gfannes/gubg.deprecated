#ifndef HEADER_gubg_parse_xml_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_xml_Parser_hpp_ALREADY_INCLUDED

#include "gubg/parse/xml/Codes.hpp"
#include "gubg/parse/xml/Coding.hpp"
#include "gubg/Strange.hpp"
#include <vector>
#include <string>
#include <map>

#define GUBG_MODULE_ "xml::Parser"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace xml
    {
        template <typename Receiver>
            class Parser_crtp
            {
                public:
                    typedef std::vector<std::string> Path;
                    typedef std::map<std::string, std::string> Attributes;

                    ReturnCode process(const std::string &str)
                    {
                        MSS_BEGIN(ReturnCode, str);

                        MSS(!str.empty(), XMLEmpty);
                        str_ = str;
                        while (str_.popCharIf('\n') || str_.popCharBack('\n') || str_.popCharIf('\r') || str_.popCharBack('\r')){}
                        path_.clear();

                        while (!str_.empty())
                        {
                            Strange text;
                            MSS(str_.popTo(text, '<'));
                            if (!text.empty())
                            {
                                L(STREAM(text));
                                MSS(!path_.empty(), TextNotExpected);
                                std::string t;
                                MSS(decode(t, text.str()));
                                receiver_().parser_text(t, path_);
                            }

                            //Check for a comment
                            {
                                Strange comment;
                                if (MSS_IS_OK(readComment_(comment)))
                                {
                                    L(STREAM(comment));
                                    receiver_().parser_comment(comment.str(), path_);
                                    continue;
                                }
                            }

                            Strange tag, attr;
                            Flags flags;
                            MSS(readTag_(tag, attr, flags));
                            L(STREAM(tag, attr, flags));
                            if (flags & Open)
                            {
                                std::string t;
                                MSS(decode(t, tag.str()));
                                receiver_().parser_open(t, path_);
                                path_.push_back(std::move(t));
                                Attributes attributes;
                                MSS(readAttributes_(attributes, attr));
                                if (!attributes.empty())
                                    receiver_().parser_attr(attributes, path_);
                            }
                            if (flags & Close)
                            {
                                MSS(!path_.empty());
                                std::string t;
                                MSS(decode(t, tag.str()));
                                L(STREAM(t, path_.back()));
                                MSS(t == path_.back(), CloseTagMismatch);
                                path_.pop_back();
                                receiver_().parser_close(std::move(t), path_);
                            }
                        }

                        MSS(path_.empty());

                        MSS_END();
                    }

                    //Default event points
                    void parser_text(const std::string &text, const Path &path){}
                    void parser_comment(const std::string &comment, const Path &path){}

                private:
                    typedef unsigned int Flags;
                    static const Flags None = 0;
                    static const Flags Open = 1;
                    static const Flags Close = 2;
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                    ReturnCode readComment_(Strange &comment)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_Q(str_.popString("<!--"));
                        MSS(str_.popUntil(comment, "-->"));
                        MSS_END();
                    }
                    ReturnCode readTag_(Strange &tag, Strange &attr, Flags &flags)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(str_.popCharIf('<'));
                        Strange all;
                        MSS(str_.popUntil(all, '>'));

                        attr.clear();
                        if (all.popCharIf('/'))
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
                    ReturnCode readAttributes_(Attributes &attributes, Strange &attr)
                    {
                        MSS_BEGIN(ReturnCode);
                        attributes.clear();
                        while (attr.popCharIf(' ')){}
                        while (!attr.empty())
                        {
                            Strange k, v;
                            MSS(attr.popUntil(k, '='));
                            while (attr.popCharIf(' ')){}
                            MSS(attr.popCharIf('"'));
                            MSS(attr.popUntil(v, '"'));
                            std::string kk, vv;
							L(STREAM(k.str(), v.str()));
                            MSS(decode(kk, k.str()));
                            MSS(decode(vv, v.str()));
                            //insert().second returns false if insertion failed (i.e., the attribute was already present)
                            //XML does not allow duplicate attribute names
                            MSS(attributes.insert(std::make_pair(kk, vv)).second, DuplicateAttributeName);
                            while (attr.popCharIf(' ')){}
                        }
                        MSS_END();
                    }

                    Strange str_;
                    Path path_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
