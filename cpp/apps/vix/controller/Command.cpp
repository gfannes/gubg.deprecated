#include "vix/controller/Command.hpp"
#include "vix/controller/Commander.hpp"
#include "vix/model/FileSystem.hpp"
#include "gubg/Platform.hpp"
#define GUBG_MODULE "Command"
#define LOG_LEVEL Warning
#include "gubg/logging/Log.hpp"
using namespace vix::controller::command;
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

CreateFile::CreateFile(Commander &commander, const string &name):
    commander_(commander),
    name_(name){}
bool CreateFile::execute()
{
    if (name_.empty())
        return false;
    std::string newSelected = name_;
    if (name_[name_.size()-1] == '/')
    {
        //Create a directory
        newSelected.resize(newSelected.size()-1);
        vix::model::FileSystem::instance().createSubDirectory(commander_.selections_.current()->path(), newSelected);
    }
    else
    {
        //Create an empty file
        vix::model::FileSystem::instance().createSubRegular(commander_.selections_.current()->path(), newSelected);
    }
    commander_.selections_.current()->setSelected(newSelected);
    return true;
}

bool Quit::execute()
{
    exit(0);
    return true;
}
