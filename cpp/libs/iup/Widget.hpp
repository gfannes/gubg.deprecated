#ifndef HEADER_iup_Widget_hpp_ALREADY_INCLUDED
#define HEADER_iup_Widget_hpp_ALREADY_INCLUDED

#include "iup/iup.hpp"
#include <vector>
#include <string>
#include <map>

namespace iup
{
    class Widget
    {
        public:
            Widget(): handle_(0), parent_(0){}

            operator Ihandle*() { return handle_; }

            template <typename K, typename V>
                void setAttribute(const K &k, const V &v) { attributes_[k] = v; }

            void addChild(Widget &w) { widgets_.push_back(&w); }

            bool create()
            {
                if (handle_)
                    return true;

                //We create our subwidgets before we create ourselves
                for (auto w: widgets_)
                    if (!w->create())
                        return false;
                //Create ourselves
                handle_ = create_();
                if (!handle_)
                    return false;
                //Set the attributes that were registered
                for (auto attr: attributes_)
                    IupSetAttribute(handle_, attr.first.c_str(), attr.second.c_str());
                return true;
            }
            void mapped()
            {
                mapped_();
                for (auto w: widgets_)
                    w->mapped();
            }

        protected:
            virtual Ihandle *create_() = 0;
            virtual void mapped_() {}

            Ihandle *handle_;
            Widget *parent_;
            typedef std::vector<Widget*> Widgets;
            Widgets widgets_;

            typedef std::string Key;
            typedef std::string Value;
            typedef std::map<Key, Value> Attributes;
            Attributes attributes_;
    };
}

#endif
