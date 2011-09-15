#include "xml.hpp"
#include <array>

//#define L_ENABLE_DEBUG
#include "debug.hpp"

std::ostream &operator<<(std::ostream &os, const gubg::xml::IChild &child)
{
    child.output_(os, 0);
    return os;
}

namespace
{
    std::string escapeForXML_(const std::string &source)
    {
        std::string escaped(source);

        typedef std::array<char, 5> SpecialCharacters;
        //The order of these special characters is important here; if you first replace '<' into "&lt;"
        //you will replace the '&' of "&lt;" the next time. Not so nice, prettify when you have time.
        SpecialCharacters specialCharacters = {'&', '<', '>', '\'', '\"'};
        typedef std::array<const char *, 5> Escapes;
        Escapes escapes = {"&amp;", "&lt;", "&gt;", "&apos;", "&quot;"};

        for (int i = 0; i < specialCharacters.size(); ++i)
        {
            size_t pos = 0;
            while (std::string::npos != (pos = escaped.find(specialCharacters[i], pos)))
            {
                DEBUG_PRINT("before: " << escaped);
                escaped.replace(pos, 1, escapes[i]);
                DEBUG_PRINT("after : " << escaped);
                ++pos;
            }
        }
        return escaped;
    }
    struct Indent
    {
        Indent(unsigned int indent):
            indent_(indent){}
        unsigned int indent_;
    };
}
std::ostream &operator<<(std::ostream &os, const Indent &indent)
{
    return os << std::string(2*indent.indent_, ' ');
}

using namespace gubg::xml;
template <>
Element &Element::operator<< <std::string> (const std::string &value)
{
    childs_.push_back(ChildPtr(new Content(value)));
    return *this;
}
Element &Element::operator()(const std::string &tag)
{
    Element *subElement = new Element(tag);
    childs_.push_back(ChildPtr(subElement));
    return *subElement;
}
template <>
Element &Element::option<std::string>(const std::string &name, const std::string &value)
{
    options_[name] = value;
    return *this;
}
void Element::output_(std::ostream &os, unsigned int indent) const
{
    if (childs_.empty())
    {
        os << Indent(indent) << "<" << tag_;
        appendOptions_(os);
        os << " />" << std::endl;
    }
    else
    {
        os << Indent(indent) << "<" << tag_;
        appendOptions_(os);
        os << ">" << std::endl;
        for (Childs::const_iterator it = childs_.begin(); it != childs_.end(); ++it)
            (*it)->output_(os, indent+1);
        os << Indent(indent) << "</" << tag_ << ">" << std::endl;
    }
}
void Element::appendOptions_(std::ostream &os) const
{
    for (Options::const_iterator it = options_.begin(); it != options_.end(); ++it)
        os << " " << it->first << "=\"" << escapeForXML_(it->second) << "\"";
}

void Content::output_(std::ostream &os, unsigned int indent) const
{
        os << Indent(indent) << escapeForXML_(value_) << std::endl;
}
