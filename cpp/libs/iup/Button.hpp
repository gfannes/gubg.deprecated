#ifndef HEADER_iup_Button_hpp_ALREADY_INCLUDED
#define HEADER_iup_Button_hpp_ALREADY_INCLUDED

#include "iup/Widget.hpp"
#include "iup/Callback.hpp"
#include "gubg/crtp.hpp"
#include "gubg/l.hpp"
#include <string>

namespace iup
{
    template <typename Receiver>
        class Button_crtp: public Widget
        {
            public:
                void setLabel(const std::string &label) { label_ = label; }
                virtual Ihandle *create_()
                {
                    Ihandle *button = IupButton(label_.c_str(), 0);
                    L("Button create_ " << button);
                    iup::Callback<Button_crtp>::create(button, "ACTION", this, &Button_crtp::clicked_);
                    return button;
                }
            private:
                GUBG_RECEIVER();
                int clicked_()
                {
                    receiver_().button_clicked();
                    return IUP_DEFAULT;
                }

                std::string label_;
        };
}

#endif
