#ifndef HEADER_gubg_xml_xml_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_xml_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>
#include <sstream>
#include <memory>
#include <vector>
#include <map>

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
        //Base class for an xlm element and content
        struct IChild
        {
            //This virtual function is used to achieve polymorphic streaming
            virtual void output_(std::ostream &, unsigned int indent) const = 0;
        };

        //Represents xml content, i.e., everything outside <...> or &...;
        struct Content: IChild
        {
            Content(const std::string &value):
                value_(value){}

            protected:
            virtual void output_(std::ostream &, unsigned int indent) const;

            private:
            std::string value_;
        };

        //Represents an xml element with a certain tag name and its childs (other xml elements and xml content):
        //<tag>
        //   content
        //   <otherTag>...</otherTag>
        //</tag>
        struct Element: IChild
        {
            Element(const std::string &tag):
                tag_(tag){}

            //Adds an xml content child to this and returns this (i.e., chainable)
            template <typename T>
            Element &operator<<(const T &value)
            {
                //In general, we use std::ostringstream to convert value into a std::string
                //Next, we use the specialization to stream the std::string
                std::ostringstream oss;
                oss << value;
                return operator<<(oss.str());
            }

            //Adds an xml element child to this and returns this _new_ xml element
            //Using something like "auto &newElement = element("tag")" to create a new subtag seems convenient
            Element &operator()(const std::string &tag);

            //Adds an option and returns this
            template <typename T>
            Element &option(const std::string &name, const T &value)
            {
                std::ostringstream oss;
                oss << value;
                return option(name, oss.str());
            }

            protected:
            virtual void output_(std::ostream &, unsigned int indent) const;

            private:
            //We don't allow copying nor assignment
            Element(const Element &);
            Element &operator=(const Element &);

            std::string tag_;

            typedef std::map<std::string, std::string> Options;
            Options options_;
            void appendOptions_(std::ostream &os) const;

            typedef std::shared_ptr<IChild> ChildPtr;
            typedef std::vector<ChildPtr> Childs;
            Childs childs_;
        };

        //Specialization for adding an std::string xml content
        template <>
        Element &Element::operator<< <std::string> (const std::string &value);

        //Specialization for adding an std::string option
        template <>
        Element &Element::option<std::string>(const std::string &name, const std::string &value);
    }
}

#endif
