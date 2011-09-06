#include "settings/Settings.hpp"
#include "Platform.hpp"

using namespace vix;
using namespace std;

bool Settings::view(const string &filepath) const
{
    return false;
}
bool Settings::edit(const string &filepath) const
{
    gubg::spawn("gvim --remote-tab-silent", gubg::escapeForCLI(filepath, gubg::Quotes::Add));
#ifdef GUBG_LINUX
    gubg::spawn("wmctrl -a GVIM");
#endif
    return true;
}
bool Settings::open(const string &filepath) const
{
#ifdef GUBG_LINUX
    gubg::spawn("gnome-open", gubg::escapeForCLI(filepath, gubg::Quotes::Add));
#endif
#ifdef GUBG_WIN32
    gubg::spawn(filepath);
#endif
    return true;
}
