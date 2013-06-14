#include "rbc/Main.hpp"
using namespace rbc;

    void selectTTY(const nana::gui::eventinfo& ei)
    {
        nana::gui::msgbox(ei.window, STR("Select a tty")).show();
    }

Main::Main():
    tty_(*this),
    arduino_dev_(*this),
    select_arduino_dev_(*this),
    main_(*this),
    log_(*this),
    place_(*this),
    picture_(*this, nana::rectangle(100,100,100,100)),
    drawing_(picture_)
{
    using namespace nana::gui;

    caption(STR("Robot control"));

    tty_.caption(STR("tty"));
    arduino_dev_.caption(STR("/dev/ttyACM0"));
    select_arduino_dev_.caption(STR("Select tty"));
    select_arduino_dev_.make_event<events::click>(selectTTY);

    main_.caption(STR("main"));

    log_.caption(STR("log"));

    place_.div("<vertical <tty><main><log>>");

    place_.field("tty") << tty_ << arduino_dev_ << select_arduino_dev_;
    place_.field("main") << main_;
    place_.field("log") << log_;

    place_.collocate();

    drawing_.line(1,1,100,100,0xff0000);
    drawing_.rectangle(50,50,10,100,0x00ff00,true);

    show();
nana::gui::API::refresh_window(picture_);
}
