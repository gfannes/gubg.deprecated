#include "controller/Command.hpp"
#include "controller/Commander.hpp"
using namespace vix::command;

bool Open::execute()
{
    switch (commander_.selection_.activateSelected(action_))
    {
        case model::Activation::Directory:
            commander_.clear();
            break;
        case model::Activation::Regular: break;
        default: break;
    }
    return true;
}

bool NewTab::execute()
{
    return true;
}

bool Quit::execute()
{
    exit(0);
    return true;
}
