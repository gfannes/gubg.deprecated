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

bool ToggleRecursiveMode::execute()
{
    commander_.selections_.current()->setRecursiveMode(!commander_.selections_.current()->getRecursiveMode());
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

CloseTab::CloseTab(Commander &commander, int ix):
    commander_(commander),
    ix_(ix){}
bool CloseTab::execute()
{
    if (commander_.selections_.nrModels() <= 1)
        exit(0);
    else
        commander_.selections_.deleteSelection(ix_);
    return true;
}

bool Quit::execute()
{
    exit(0);
    return true;
}
