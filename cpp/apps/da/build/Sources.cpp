#include "da/build/Sources.hpp"
#include <algorithm>
using namespace da;
using namespace std;

namespace 
{
    struct CompareFile
    {
        const Source::File wantedFile;
        CompareFile(Source::File f):wantedFile(f){}
        bool operator()(Source::Ptr src)
        {
            return src->file().name() == wantedFile.name();
        }
    };
}
Source::Ptr Sources::get(Source::File file) const
{
    auto it = find_if(sources_.begin(), sources_.end(), CompareFile(file));
    if (it == sources_.end())
        return Source::Ptr();
    return *it;
}

Source::Ptr Sources::add(Source::File file)
{
    Source::Ptr ret;
    auto it = find_if(sources_.begin(), sources_.end(), CompareFile(file));
    if (it == sources_.end())
    {
        verbose("Adding new file", file.name());
        ret = Source::create(file);
        sources_.push_back(ret);
    }
    else
    {
        ret = *it;
    }
    return ret;
}
