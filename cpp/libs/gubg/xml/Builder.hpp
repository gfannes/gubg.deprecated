#ifndef HEADER_gubg_xml_Builder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_Builder_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg
{
    namespace xml
    {
        namespace builder
        {
			enum CloseType {ShortClose, NoShortClose};
            class Tag
            {
                public:
                    Tag(std::ostream &os, const std::string &name, CloseType closeType = ShortClose, const size_t indent = 0)
                        :os_(os), name_(name), indent_(indent), state_(Attributes), closeType_(closeType)
                    {
                        if (indent_)
                            os_ << std::string(2*indent_, ' ');
                        os_ << "<" << name_;
                    }
                    Tag(const Tag &&rhs):
                        os_(rhs.os_),
                        name_(std::move(rhs.name_)),
                        indent_(rhs.indent_),
                        state_(rhs.state_),
						closeType_(rhs.closeType_)
                    {
                    }
                    template <typename Name, typename Value>
                        Tag &attr(const Name &name, const Value &value)
                        {
                            os_ << " " << name << "=" << "\"" << value << "\"";
                            return *this;
                        }
                    Tag tag(const std::string &name)
                    {
                        closeAttributes_();
                        os_ << std::endl;
                        return Tag(os_, name, closeType_, indent_+1);
                    }
					template <typename T>
						Tag &operator<<(const T &t)
						{
							closeAttributes_();
							os_ << t;
							return *this;
						}
					~Tag()
					{
						switch (state_)
						{
                            case Attributes:
								switch (closeType_)
								{
									case ShortClose:
										os_ << "/>";
										break;
									case NoShortClose:
										os_ << "></" << name_ << ">";
										break;
								}
                                break;
                            case Elements:
                                os_ << std::endl << std::string(2*indent_, ' ');
                                os_ << "</" << name_ << ">";
                                break;
                        }
                    }

                private:
                    Tag(const Tag &);

                    void closeAttributes_()
                    {
                        if (state_ != Attributes)
                            return;
                        os_ << ">";
                        state_ = Elements;
                    }

                    std::ostream &os_;
                    const std::string name_;
                    const size_t indent_;
                    enum State {Attributes, Elements, Closed};
                    State state_;
					CloseType closeType_;
            };
            class Header
            {
                public:
                    Header(std::ostream &os): os_(os) { os_ << "<?xml"; }
                    ~Header() { os_ << "?>" << std::endl; }
                    template <typename Name, typename Value>
                        Header &attr(const Name &name, const Value &value)
                        {
                            os_ << " " << name << "=" << "\"" << value << "\"";
                            return *this;
                        }
                private:
                    std::ostream &os_;
            };
            class Comment
            {
                public:
                    Comment(std::ostream &os): os_(os) { os_ << "<!--"; }
                    ~Comment() { os_ << "-->" << std::endl; }
                    template <typename T>
                        Comment &operator<<(const T &t) {os_ << t; return *this;}
                private:
                    std::ostream &os_;
            };
        }
    }
}

#endif
