#include "model/Selection.hpp"
#include "settings/Settings.hpp"
#define GUBG_MODULE "Selection"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
#include "nullptr.hpp"
#include "boost/algorithm/string/replace.hpp"
#include <algorithm>
using namespace vix::model;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

#define L_LOCK() Lock l(lock())

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
    L_LOCK();
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
    L_LOCK();
    return current_;
}
int Selections::nrModels() const
{
    L_LOCK();
    return selections_.size();
}
void Selections::setCurrent(int ix)
{
    L_LOCK();
    if (ix < 0 || ix >= selections_.size())
        return;
    LOG_SM_(Debug, setCurrent, "ix: " << ix << ", current_: " << current_);
    current_ = ix;
    LOG_M_(Debug, "current_: " << current_ << ", current(): " << current());
    updated_(current());
}
void Selections::addSelection(const string &path)
{
    LOG_SM_(Debug, addSelection, "path: " << path);
    L_LOCK();
    current_ = selections_.size();
    selections_.push_back(new Selection(*this, path));
}
void Selections::deleteSelection(int ix)
{
    L_LOCK();
    if (ix < 0 || ix >= selections_.size() || selections_.size() <= 1)
        return;
    LOG_SM_(Debug, deleteSelection, "ix: " << ix << ", current_: " << current_ << " current() " << current());
    if (ix < current_)
        --current_;
    else if (ix == current_ && ix == selections_.size()-1)
        --current_;
    selections_.erase(selections_.begin()+ix);
    LOG_M_(Debug, "current_: " << current_ << ", current(): " << current());
    updated_(current());
}
boost::signals2::connection Selections::connect(const UpdateSignal::slot_type &subscriber)
{
    L_LOCK();
    return updated_.connect(subscriber);
}

//Selection
Selection::Selection(Selections &selections, const string &path):
    selections_(selections),
    selectedIX_(InvalidIX),
    recursive_(false),
    consumerThread_(&Selection::consumer_, this)
{
    FileSystem &filesystem = FileSystem::instance();
    auto p = filesystem.getPath(path);
    if (!p)
        p = filesystem.getPath("/");

    Message::Ptr message(new Message);
    message->path = p;
    queue_.push(std::move(message));
}
Selection::~Selection()
{
    queue_.close();
    consumerThread_.join();
}

std::vector<Selection*> Selection::selections()
{
    boost::mutex::scoped_lock lock(selectedMutex_);
    return selections_.selections_;
}

void Selection::setPath(Path path)
{
    Message::Ptr message(new Message);
    message->path = path;
    queue_.push(std::move(message));
}

void Selection::setNameFilter(const string &nameFilter)
{
    LOG_SM_(Debug, setNameFilter, "Setting nameFilter to " << nameFilter);
    Message::Ptr message(new Message);
    message->nameFilter.reset(new string(nameFilter));
    queue_.push(std::move(message));
}
string Selection::getNameFilter() const
{
    boost::mutex::scoped_lock lock(filesMutex_);
    return nameFilter_;
}

void Selection::setContentFilter(const string &contentFilter)
{
    LOG_SM_(Debug, setContentFilter, "Setting contentFilter to " << contentFilter);
    Message::Ptr message(new Message);
    message->contentFilter.reset(new string(contentFilter));
    queue_.push(std::move(message));
}
string Selection::getContentFilter() const
{
    boost::mutex::scoped_lock lock(filesMutex_);
    return contentFilter_;
}

void Selection::setRecursiveMode(bool recursive)
{
    LOG_SM_(Debug, setRecursive, "Setting recursive mode " << (recursive ? "ON" : "OFF"));
    Message::Ptr message(new Message);
    message->recursive.reset(new bool(recursive));
    queue_.push(std::move(message));
}
bool Selection::getRecursiveMode() const
{
    boost::mutex::scoped_lock lock(filesMutex_);
    return recursive_;
}

bool Selection::getContent(string &content, Format format)
{
    LOG_SM_(Debug, getContent, "");
    gubg::file::Regular *regular = 0;
    {
        boost::mutex::scoped_lock lock1(filesMutex_);
        boost::mutex::scoped_lock lock2(selectedMutex_);
        if (files_.empty() || selectedIX_ < 0 || selectedIX_ >= files_.size())
        {
            LOG_M_(Debug, "No current file present");
            return false;
        }
        regular = dynamic_cast<gubg::file::Regular*>(files_[selectedIX_].operator->().get());
    }
    if (!regular)
    {
        LOG_M_(Debug, "This is not a regular file");
        return false;
    }

    //Load the content of the file
    if (!regular->load(content))
    {
        LOG_M_(Error, "Could not load the content");
        return false;
    }

    //Check the size of the file
    if (content.size() > 500000)
    {
        LOG_M_(Warning, "File is too large to show");
        return false;
    }

    switch (format)
    {
        case Format::Html:
            boost::replace_all(content, "\n", "<br/>");
            break;
        default:
            return false;
            break;
    }
    return true;
}

void Selection::setSelected(const string &selected)
{
    Message::Ptr message(new Message);
    message->selected.reset(new string(selected));
    queue_.push(std::move(message));
}
string Selection::getSelected() const
{
    boost::mutex::scoped_lock lock(selectedMutex_);
    return selected_;
}

void Selection::getFiles(Files &files, int &selectedIX) const
{
    boost::mutex::scoped_lock lock1(filesMutex_);
    boost::mutex::scoped_lock lock2(selectedMutex_);
    files = files_;
    selectedIX = selectedIX_;
}

Activation Selection::activateSelected(Action action)
{
    boost::mutex::scoped_lock lock1(filesMutex_);
    boost::mutex::scoped_lock lock2(selectedMutex_);
    if (files_.empty() || InvalidIX == selectedIX_ || selectedIX_ < 0)
        return Activation::Error;
    auto selected = files_[selectedIX_];
    LOG_SM_(Debug, activateSelected, "path_: " << path_ << ", selected: " << selected);
    if (auto newPath = FileSystem::instance().toPath(selected))
    {
        LOG_M_(Debug, "This is a directory");
        Message::Ptr message(new Message);
        message->path = newPath;
        queue_.push(std::move(message));
        return Activation::Directory;
    }
    if (auto file = FileSystem::instance().toRegular(selected))
    {
        LOG_M_(Debug, "This is a regular file");
        switch (action)
        {
            case Action::View:
                vix::Settings::instance().view(file->filepath());
                break;
            case Action::Edit:
                vix::Settings::instance().edit(file->filepath());
                break;
            case Action::Open:
                vix::Settings::instance().open(file->filepath());
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

void Selection::move(Direction direction)
{
    LOG_S_(Debug, move);
    Message::Ptr message(new Message);
    message->direction.reset(new Direction(direction));
    queue_.push(std::move(message));
}

//Private methods

//Collect all entries in path_ that match our current nameFilter and store them into files_
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
    if (!FileSystem::instance().getFiles(allFiles, path_, recursive_))
    {
        LOG_M_(Warning, "Could not get the files");
        return;
    }
    
    if (recursive_)
    {
    }
    else
    {
        files_.clear();
        for (auto it = allFiles.begin(); it != allFiles.end(); ++it)
        {
            auto &file = *it;
            File::Unlock unlockedFile(file);
            if (!unlockedFile->isHidden())
            {
                smatch match;
                if (!reNameFilter_ || regex_search(unlockedFile->name(), match, *reNameFilter_))
                    files_.push_back(file);
            }
        }
    }
    std::sort(files_.begin(), files_.end(), CmpLess());
    LOG_M_(Debug, "I selected " << files_.size() << " out of " << allFiles.size());
}

void Selection::updateSelected_()
{
    LOG_SM_(Debug, updateSelected_, "selected_: " << selected_);
    //First, we try to match based on selected_
    auto six = InvalidIX;
    if (!selected_.empty())
    {
        for (auto it = files_.begin(); it != files_.end(); ++it)
        {
            auto &file = *it;
            if (selected_ == file->name())
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
            selected_ = files_[six]->name();
        }
    }
    selectedIX_ = six;
    selectedPerPath_[path_->path()] = selected_;
    LOG_M_(Debug, "selectedIX_: " << selectedIX_ << ", selected_: " << selected_ << " selectedPerPath_.size(): " << selectedPerPath_.size());
}

void Selection::consumer_()
{
    LOG_SM_(Debug, consumer_, "Thread is starting");
    try
    {
        while (true)
        {
            LOG_M_(Debug, "Waiting for a message...");
            auto ptr = queue_.pop();
            Message &message(*ptr);
            LOG_M_(Debug, "I received a message!");

            //Drastic locking, should be OK for now
            boost::mutex::scoped_lock lock1(filesMutex_);
            boost::mutex::scoped_lock lock2(selectedMutex_);

            bool doUpdateFiles = false;
            bool doUpdateSelected = false;

            //Check if we have to update the current Files list
            if (message.nameFilter.get() && *message.nameFilter != nameFilter_)
            {
                LOG_SM_(Debug, nameFilter, "");
                nameFilter_ = *message.nameFilter;
                if (nameFilter_.empty())
                    reNameFilter_.reset();
                else
                    reNameFilter_.reset(new regex(nameFilter_, regex_constants::icase));
                doUpdateFiles = true;
            }
            if (message.contentFilter.get() && *message.contentFilter != contentFilter_)
            {
                LOG_SM_(Debug, contentFilter, "");
                contentFilter_ = *message.contentFilter;
                if (contentFilter_.empty())
                    reContentFilter_.reset();
                else
                    reContentFilter_.reset(new regex(contentFilter_, regex_constants::icase));
                doUpdateFiles = true;
            }
            if (message.path())
            {
                LOG_SM_(Debug, path, "");
                path_ = message.path;
                auto p = path_->path();
                auto it = selectedPerPath_.find(path_->path());
                if (it != selectedPerPath_.end())
                {
                    selected_ = it->second;
                    doUpdateSelected = true;
                    LOG_M_(Debug, "I found a preferred selected_ for this path: " << selected_);
                }
                doUpdateFiles = true;
            }
            if (message.recursive.get() && *message.recursive != recursive_)
            {
                LOG_SM_(Debug, recursive, "");
                recursive_ = *message.recursive;
                doUpdateFiles = true;
            }
            if (doUpdateFiles)
            {
                LOG_SM_(Debug, updateFiles, "");
                updateFiles_();
            }

            //Check if we have to update the currently selected File
            //Move
            if (message.direction.get())
            {
                LOG_SM_(Debug, move, "");
                if (!path_->empty() && InvalidIX != selectedIX_)
                {
                    switch (*message.direction)
                    {
                        case Direction::Up:
                            if (selectedIX_ > 0)
                            {
                                --selectedIX_;
                                selected_.clear();
                                doUpdateSelected = true;
                            }
                            break;
                        case Direction::Down:
                            if (selectedIX_ < files_.size()-1)
                            {
                                ++selectedIX_;
                                selected_.clear();
                                doUpdateSelected = true;
                            }
                            break;
                    }
                }
            }
            //New selected value
            if (message.selected.get())
            {
                LOG_SM_(Debug, selected, "");
                selected_ = *message.selected;
                doUpdateSelected = true;
            }
            //If the files themselves change, we have to update the selected File too
            if (doUpdateSelected || doUpdateFiles)
            {
                LOG_SM_(Debug, updateSelected, "");
                updateSelected_();
            }

            //If either the files or the selected file got updated, we have to report this to our subscribers
            if (doUpdateFiles || doUpdateSelected)
            {
                LOG_SM_(Debug, signalSubscribers, "");
                selections_.updated_(this);
            }
        }
    }
    catch (Selection::QueueT::Closed&)
    {
        LOG_M_(Debug, "The queue is closed");
    }
    catch (...)
    {
        LOG_M_(Debug, "Received an unknown error");
        throw;
    }
    LOG_M_(Debug, "Thread is stopping");
}
