#ifndef HEADER_iup_Label_hpp_ALREADY_INCLUDED
#define HEADER_iup_Label_hpp_ALREADY_INCLUDED

#include "iup/Widget.hpp"
#include <string>

namespace iup
{
    class Label: public Widget
    {
        public:
            Label(){}
            Label(const std::string &label):label_(label){}

            void setLabel(const std::string &label) { label_ = label; }

            virtual Ihandle *create_()
            {
                return IupLabel(label_.c_str());
            }
        private:
            std::string label_;
    };
}

#endif
