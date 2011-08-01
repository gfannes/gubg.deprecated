#include "model/Selection.hpp"
#include "settings/Settings.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
#include "nullptr.hpp"
#include <algorithm>
using namespace vix::model;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

//Selections
Selections::Selections():
    current_(-1){}
Selections::~Selections()
{
    for (auto selection = selections_.begin(); selection != selections_.end(); ++selection)
        delete *selection;
}
bool Selections::empty() const
{
    return selections_.empty();
}
Selection *Selections::current() const
{
    if (current_ < 0)
        return nullptr;
    return selections_[current_];
}
int Selections::currentIX() const
{
    return current_;
}
int Selections::nrModels() const
{
    return selections_.size();
}
void Selections::setCurrent(int ix)
{
    if (ix < 0 || ix >= selections_.size())
        return;
    LOG_SM_(Debug, Selections::setCurrent, "ix: " << ix << ", current_: " << current_);
    current_ = ix;
    LOG_M_(Debug, "current_: " << current_ << ", current(): " << current());
    updated_(current());
}
void Selections::addSelection(const string &path)
{
    selections_.push_back(new Selection(*this, path));
    current_ = selections_.size()-1;
    updated_(current());
}
boost::signals2::connection Selections::connect(const UpdateSignal::slot_type &subscriber)
{
    return updated_.connect(subscriber);
}

//Selection
Selection::Selection(Selections &selections, const string &path):
    selections_(selections),
    selectedIX_(InvalidIX)
{
    FileSystem &filesystem = FileSystem::instance();
    path_ = filesystem.getPath(path);

    if (!path_)
        path_ = filesystem.getPath("/");
    updateFiles_();
    updateSelection_();
}

std::vector<Selection*> Selection::selections()
{
    return selections_.selections_;
}

void Selection::setPath(Path path)
{
    path_ = path;
    updateFiles_();
    selections_.updated_(this);
}
void Selection::setFilter(const string &filter)
{
    LOG_SM_(Debug, setFilter, "Setting filter to " << filter);
    filter_ = filter;
    if (filter_.empty())
        reFilter_.reset();
    else
        reFilter_.reset(new regex(filter_, regex_constants::icase));
    updateFiles_();
    updateSelection_(selected_);
    selections_.updated_(this);
}
void Selection::setSelected(const string &selected)
{
    updateSelection_(selected);
    selections_.updated_(this);
}
string Selection::getSelected() const
{
    return selected_;
}

void Selection::getFiles(Files &files, int &selectedIX) const
{
    files = files_;
    selectedIX = selectedIX_;
}

Activation Selection::activateSelected(Action action)
{
    if (files_.empty() || InvalidIX == selectedIX_ || selectedIX_ < 0)
        return Activation::Error;
    auto selected = files_[selectedIX_];
    LOG_SM_(Debug, gotoSelected, "path_: " << path_ << ", selected: " << selected);
    if (auto newPath = FileSystem::instance().toPath(selected))
    {
        LOG_M_(Debug, "This is a directory");
        path_ = newPath;
        updateFiles_();
        LOG_M_(Debug, "path_ is now: " << path_);
        updateSelection_();
        selections_.updated_(this);
        return Activation::Directory;
    }
    if (auto file = FileSystem::instance().toRegular(selected))
    {
        LOG_M_(Debug, "This is a regular file");
        Regular::Unlock unlockedRegular(file);
        switch (action)
        {
            case Action::View:
                vix::Settings::instance().view(unlockedRegular->filepath());
                break;
            case Action::Edit:
                vix::Settings::instance().edit(unlockedRegular->filepath());
                break;
            case Action::Open:
                vix::Settings::instance().open(unlockedRegular->filepath());
                break;
            default:
                LOG_M_(Error, "This action is not yet implemented");
                return Activation::Error;
                break;
        }
        return Activation::Regular;
    }
    return Activation::Error;
}

bool Selection::move(Direction direction)
{
    LOG_S_(Debug, move);
    {
        Path::Unlock unlockedPath(path_);
        if (unlockedPath->empty())
            return false;
    }
    if (InvalidIX == selectedIX_)
        return false;
    switch (direction)
    {
        case Direction::Up:
            if (selectedIX_ <= 0)
                return false;
            --selectedIX_;
            break;
        case Direction::Down:
            if (selectedIX_ >= files_.size()-1)
                return false;
            ++selectedIX_;
            break;
    }
    updateSelection_();
    selections_.updated_(this);
    return true;
}

//Private methods

//Collect all entries in path_ that match our current filter and store them into files_
namespace
{
    struct CmpLess
    {
        bool operator()(const File &l, const File &r) const
        {
            File::Unlock unlockedLhs(l);
            auto lhs = unlockedLhs.ptr();
            File::Unlock unlockedRhs(r);
            auto rhs = unlockedRhs.ptr();
            if (lhs->isDirectory() && rhs->isRegular())
                return true;
            if (rhs->isDirectory() && lhs->isRegular())
                return false;
            return lhs->name() < rhs->name();
        }
        private:
    };
}
void Selection::updateFiles_()
{
    LOG_SM_(Debug, updateFiles_, "path_: " << path_);
    Files allFiles;
    if (!FileSystem::instance().getFiles(allFiles, path_))
    {
        LOG_M_(Warning, "Could not get the files");
        return;
    }
    
    files_.clear();
    for (auto it = allFiles.begin(); it != allFiles.end(); ++it)
    {
        auto &file = *it;
        File::Unlock unlockedFile(file);
        if (!unlockedFile->isHidden())
        {
            smatch match;
            if (!reFilter_ || regex_search(unlockedFile->name(), match, *reFilter_))
                files_.push_back(file);
        }
    }
    std::sort(files_.begin(), files_.end(), CmpLess());
    LOG_M_(Debug, "I selected " << files_.size() << " out of " << allFiles.size());
}

void Selection::updateSelection_(const std::string &selected)
{
    selected_ = selected;
    LOG_SM_(Debug, Selection::updateSelection_, "selected_: " << selected_);
    //First, we try to match based on selected_
    auto six = InvalidIX;
    if (!selected_.empty())
    {
        for (auto it = files_.begin(); it != files_.end(); ++it)
        {
            auto &file = *it;
            File::Unlock unlockedFile(file);
            if (selected_ == unlockedFile->name())
            {
                six = it - files_.begin();
                LOG_M_(Debug, "I found " << selected_ << " at ix " << six);
                break;
            }
        }
    }

    //If that fails, we take a new selection close to the previous selectedIX_, if possible
    if (InvalidIX == six)
    {
        if (files_.empty())
            six = InvalidIX;
        else
        {
            if (InvalidIX == selectedIX_)
                six = 0;
            else if (selectedIX_ < 0)
                six = 0;
            else if (selectedIX_ >= files_.size())
                six = files_.size()-1;
            else
                six = selectedIX_;
            File::Unlock unlockedFile(files_[six]);
            selected_ = unlockedFile->name();
        }
    }
    selectedIX_ = six;
    LOG_M_(Debug, "selectedIX_: " << selectedIX_ << ", selected_: " << selected_);
}
