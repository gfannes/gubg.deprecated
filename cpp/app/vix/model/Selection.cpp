#include "model/Selection.hpp"
#include <algorithm>
using namespace vix::model;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

Selection::Selection(const string &path):
    path_(path){}

void Selection::setPath(const string &path)
{
    path_ = path;
    updated_();
}

void Selection::getFiles(Files &files, int &selectedIX) const
{
    files.clear();
    for (auto it = directory_iterator(path_); it != directory_iterator(); ++it)
    {
        File file(it->path());
        if (!file.isHidden())
        {
            smatch match;
            if (!filter_ || regex_search(file.name, match, *filter_))
                files.push_back(file);
        }
    }
    std::sort(files.begin(), files.end());
    selectedIX = 0;
}

void Selection::setFilter(const string &filter)
{
    if (filter.empty())
        filter_.reset();
    else
        filter_.reset(new regex(filter, regex_constants::icase));
    updated_();
}

boost::signals2::connection Selection::connect(const UpdateSignal::slot_type &subscriber)
{
    return updated_.connect(subscriber);
}
