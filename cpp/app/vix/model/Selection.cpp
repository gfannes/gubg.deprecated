#include "model/Selection.hpp"
#include <algorithm>
using namespace vix::model;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

Selection::Selection(const string &path):
    path_(path){}

Files Selection::getFiles() const
{
    Files files;
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
    return files;
}

void Selection::setFilter(const string &filter)
{
    if (filter.empty())
        filter_.reset();
    else
        filter_.reset(new regex(filter, regex_constants::icase));
}
