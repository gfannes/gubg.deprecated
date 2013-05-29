#include "nana/gui/wvl.hpp"

int main()
{
    using namespace nana::gui;
    form f;
    f.caption(STR("Robot control"));
    f.show();
    exec();
    return 0;
}
