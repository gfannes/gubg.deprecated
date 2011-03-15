#include "xml.hpp"

std::ostream &operator<<(std::ostream &os, const gubg::xml::IChild &child)
{
    child.output_(os, 0);
    return os;
}

namespace
{
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
Element &Element::operator<<(const std::string &value)
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
void Element::output_(std::ostream &os, unsigned int indent) const
{
    if (childs_.empty())
    {
        os << Indent(indent) << "<" << tag_ << " />" << std::endl;
    }
    else
    {
        os << Indent(indent) << "<" << tag_ << ">" << std::endl;
        for (Childs::const_iterator it = childs_.begin(); it != childs_.end(); ++it)
            (*it)->output_(os, indent+1);
        os << Indent(indent) << "</" << tag_ << ">" << std::endl;
    }
}

void Content::output_(std::ostream &os, unsigned int indent) const
{
    os << Indent(indent) << value_ << std::endl;
}
