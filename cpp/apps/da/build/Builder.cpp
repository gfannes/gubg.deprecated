#define GUBG_LOG
#include "da/build/Builder.hpp"
#include <queue>
using namespace da;
using gubg::file::File;
using namespace std;

ReturnCode Builder::process(const SourceFile &source)
{
    MSS_BEGIN(ReturnCode, process);

    sources_.clear();

    queue<File> staging;
    staging.push(source);
    while (!staging.empty())
    {
        File srcfile = staging.front();
        staging.pop();
        if (sources_.get(srcfile))
            //This source file is already used
            continue;
        auto src = sources_.add(srcfile);

        Headers headers;
        IncludePaths includePaths;
        SourceFiles sisterFiles;
        MSS(src->searchForHeaders(headers, includePaths, sisterFiles, configuration_.packages()));
        headersPerSource_[src] = headers;
        includePaths_.insert(includePaths);
        for (auto ip: includePaths_)
            LOG_M(ip.name());

        for (auto hdr: headers)
        {
            switch (auto rc = headers_.add(hdr))
            {
                case ReturnCode::HeaderAlreadyKnown:
                    continue;
                    break;
                default: MSS(rc); break;
            }
            for (auto sisterFile: sisterFiles)
                staging.push(sisterFile);
        }
    }

    MSS_END();
}

Headers Builder::headers(Source::Ptr src) const
{
    const auto it = headersPerSource_.find(src);
    if (it == headersPerSource_.end())
        return Headers();
    return it->second;
}
