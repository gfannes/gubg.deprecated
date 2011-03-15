#ifndef gubg_xml_hpp
#define gubg_xml_hpp

#include <string>
#include <ostream>
#include <memory>
#include <vector>

namespace gubg
{
    namespace xml
    {
        struct IChild;
    }
}
std::ostream &operator<<(std::ostream &, const gubg::xml::IChild &);

namespace gubg
{
    namespace xml
    {
        struct IChild
        {
            virtual void output_(std::ostream &, unsigned int indent) const = 0;
        };
        typedef std::shared_ptr<IChild> ChildPtr;
        struct Content: IChild
        {
            Content(const std::string &value):
                value_(value){}
            protected:
            virtual void output_(std::ostream &, unsigned int indent) const;
            private:
            std::string value_;
        };
        struct Element: IChild
        {
            Element(const std::string &tag):
                tag_(tag){}

            Element &operator<<(const std::string &value)
            {
                childs_.push_back(ChildPtr(new Content(value)));
                return *this;
            }
            Element &operator()(const std::string &tag)
            {
                Element *subElement = new Element(tag);
                childs_.push_back(ChildPtr(subElement));
                return *subElement;
            }

            protected:
            virtual void output_(std::ostream &, unsigned int indent) const;
            private:
            Element(const Element &);
            Element &operator=(const Element &);
            std::string tag_;
            typedef std::vector<ChildPtr> Childs;
            Childs childs_;
        };
    }
}

#endif
