#include "da/build/Headers.hpp"
using namespace da;

namespace 
{
    struct CompareFile
    {
        const Header::File wantedFile;
        CompareFile(const Header &hdr):wantedFile(hdr.file()){}
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
    auto it = find_if(headers_.begin(), headers_.end(), CompareFile(hdr));
    MSS(it == headers_.end(), HeaderAlreadyKnown);
    verbose("Adding new header", hdr.file().name());
    headers_.push_back(ptr);
    MSS_END();
}
