#include "model/Selection.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
#include <algorithm>
using namespace vix::model;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

Selection::Selection(const string &path):
    path_(path),
    selectedIX_(InvalidIX)
    {
        updateFiles_();
        updateSelection_();
    }

void Selection::setPath(const string &path)
{
    path_ = path;
    updateFiles_();
    updated_();
}
void Selection::setFilter(const string &filter)
{
    LOG_SM_(Debug, setFilter, "Setting filter to " << filter);
    if (filter.empty())
        filter_.reset();
    else
        filter_.reset(new regex(filter, regex_constants::icase));
    updateFiles_();
    updateSelection_();
    updated_();
}
void Selection::setSelected(const string &selected)
{
    selected_ = selected;
    updateSelection_();
    updated_();
}

void Selection::getFiles(Files &files, int &selectedIX) const
{
    files = files_;
    selectedIX = selectedIX_;
}

bool Selection::gotoSelected()
{
    LOG_SM_(Debug, gotoSelected, "path_: " << path_ << ", selected_: " << selected_);
    auto newPath = path_ / selected_;
    if (!is_directory(newPath))
    {
        LOG_M_(Debug, "This is not a directory");
        return false;
    }

    newPath.swap(path_);
    updateFiles_();
    LOG_M_(Debug, "path_ is now: " << path_);

    selected_ = "";
    updateSelection_();

    updated_();
    return true;
}

bool Selection::move(Direction direction)
{
    LOG_S_(Debug, move);
    if (files_.empty())
        return false;
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
    selected_ = "";
    updateSelection_();
    updated_();
    return true;
}

boost::signals2::connection Selection::connect(const UpdateSignal::slot_type &subscriber)
{
    return updated_.connect(subscriber);
}

//Private methods

//Collect all entries in path_ that match our current filter and store them into files_
void Selection::updateFiles_()
{
    LOG_SM_(Debug, updateFiles_, "path_: " << path_);
    files_.clear();
    for (auto it = directory_iterator(path_); it != directory_iterator(); ++it)
    {
        File file(it->path());
        if (!file.isHidden())
        {
            smatch match;
            if (!filter_ || regex_search(file.name, match, *filter_))
                files_.push_back(file);
        }
    }
    std::sort(files_.begin(), files_.end());
}

void Selection::updateSelection_()
{
    LOG_SM_(Debug, updateSelection_, "selected_: " << selected_);
    //First, we try to match based on selected_
    auto six = InvalidIX;
    if (!selected_.empty())
    {
        for (auto it = files_.begin(); it != files_.end(); ++it)
        {
            if (selected_ == it->name)
            {
                six = it-files_.begin();
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
            selected_ = files_[six].name;
        }
    }
    selectedIX_ = six;
}
