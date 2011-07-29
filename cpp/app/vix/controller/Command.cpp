#include "controller/Command.hpp"
#include "controller/Commander.hpp"
using namespace vix::command;
using namespace std;

bool Open::execute()
{
    if (commander_.selections_.empty())
        return false;
    switch (commander_.selections_.current()->activateSelected(action_))
    {
        case model::Activation::Directory:
            commander_.clear();
            break;
        case model::Activation::Regular: break;
        default: break;
    }
    return true;
}

NewTab::NewTab(Commander &commander, const string &name):
    commander_(commander),
    name_(name){}
bool NewTab::execute()
{
    commander_.selections_.addSelection(name_);
    commander_.clear();
    return true;
}

bool Quit::execute()
{
    exit(0);
    return true;
}
