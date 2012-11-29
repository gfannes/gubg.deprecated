//#define GUBG_LOG
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
        const auto &forest = configuration_.forest();
        MSS(src->searchForHeaders(headers, forest));

        for (auto hdr: headers)
        {
            switch (auto rc = headers_.add(hdr))
            {
                case ReturnCode::HeaderAlreadyKnown:
                    continue;
                    break;
                default: MSS(rc); break;
            }
            auto sisterSource = hdr->file();
            sisterSource.setExtension("cpp");
            LOG_M(STREAM(sisterSource.name()));
            if (forest.contains(sisterSource))
                staging.push(sisterSource);
        }
    }

    MSS_END();
}
