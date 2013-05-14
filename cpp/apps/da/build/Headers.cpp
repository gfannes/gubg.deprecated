#include "da/build/Headers.hpp"
#include <algorithm>
using namespace da;
using namespace std;

#define GUBG_MODULE "Headers"
#include "gubg/log/begin.hpp"
namespace 
{
    struct CompareFile
    {
        const Header::File wantedFile;
        CompareFile(const Header::File &wf):wantedFile(wf){}
        bool operator()(Header::Ptr hdr)
        {
            return hdr->file().name() == wantedFile.name();
        }
    };
}
ReturnCode Headers::add(Header::Ptr ptr)
{
    MSS_BEGIN(ReturnCode);
    MSS(ptr);
    auto &hdr = *ptr;
    auto it = find_if(headers_.begin(), headers_.end(), CompareFile(hdr.file()));
    MSS_Q(it == headers_.end(), HeaderAlreadyKnown);
    headers_.push_back(ptr);
    MSS_END();
}
Header::Ptr Headers::add(Header::File file)
{
    Header::Ptr ret;
    auto it = find_if(headers_.begin(), headers_.end(), CompareFile(file));
    if (it == headers_.end())
    {
        ret = Header::create(file);
        headers_.push_back(ret);
    }
    else
    {
        ret = *it;
    }
    return ret;
}
