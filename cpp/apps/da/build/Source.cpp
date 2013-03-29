#include "da/build/Source.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#include <queue>
#include <set>
using namespace da;
using gubg::file::File;
using gubg::file::Forest;
using namespace std;

#define GUBG_MODULE "Source"
#include "gubg/log/begin.hpp"
Source::Ptr Source::create(File file)
{
    return Ptr(new Source(file));
}
Source::Source(File file):
    file_(file)
{
}

namespace 
{
    struct IncludePusher: gubg::parse::cpp::Includes_crtp<IncludePusher>, gubg::Verbose<true>
    {
        queue<File> &staging;
        IncludePusher(queue<File> &st):staging(st){}

        template <typename String, typename Type>
            void includes_detected(const String &str, Type type)
            {
                staging.push(File(str));
            }
    };
}
ReturnCode Source::searchForHeaders(Headers &headers, SourceFiles &sisterFiles, package::Packages &packages)
{
    MSS_BEGIN(ReturnCode, file().name());

    queue<File> staging;
    //Add the direct includes from file() to staging using IncludePusher
    IncludePusher includePusher(staging);
    verbose("Processing top-level", file().name());
    MSS(includePusher.process(file()));

    set<File::Name> processed;

    sisterFiles.clear();
    while (!staging.empty())
    {
        File hdr;
        SourceFiles sfs;
        {
            const auto f = staging.front();
            staging.pop();
            if (!gubg::mss::isOK(packages.resolveHeader(hdr, sfs, f)))
                //This header could not be found in the packages
                continue;
        }

        if (processed.count(hdr.name()))
            //This header is already processed
            continue;
        processed.insert(hdr.name());

        headers.add(hdr);
        sisterFiles.insert(sfs);

        MSS(includePusher.process(hdr));
    }

    MSS_END();
}
