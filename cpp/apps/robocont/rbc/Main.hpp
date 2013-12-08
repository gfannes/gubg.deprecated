#ifndef HEADER_robocont_rbc_Main_hpp_ALREADY_INCLUDED
#define HEADER_robocont_rbc_Main_hpp_ALREADY_INCLUDED

#include "nana/gui/wvl.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nana/gui/widgets/picture.hpp"

namespace rbc
{
    class Main: public nana::gui::form
    {
        public:
            Main();

        private:
            nana::gui::label tty_;
            nana::gui::label arduino_dev_;
            nana::gui::button select_arduino_dev_;

            nana::gui::label main_;

            nana::gui::label log_;

            nana::gui::place place_;

            nana::gui::picture picture_;
            nana::gui::drawing drawing_;
    };
}

#endif
