#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
#include <string>
#include <vector>
#include <algorithm>

auto s = nana::rectangle(0, 0, 1365, 800);
auto s2 = nana::rectangle(0, 200, 1365, 800);

int main()
{
    using namespace nana::gui;

    form fm(s);
    fm.caption(STR("Rinle"));
    std::string tmp;
    gubg::file::read(tmp, gubg::file::File(__FILE__));
    nana::string content(tmp.size(), '\0');
    std::copy(tmp.begin(), tmp.end(), content.begin());
    label lbl(fm, s);
    lbl.caption(content);
    lbl.make_event<events::click>(API::exit);
    label lbl2(fm, s2);
    lbl2.caption(content);
    fm.show();
    exec();
} 
