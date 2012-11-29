#include "da/build/Builder.hpp"
#include <list>
using namespace da;
using namespace gubg::file;
using namespace std;

ReturnCode Builder::process(const SourceFile &source)
{
    MSS_BEGIN(ReturnCode);

    sources_.clear();

    list<File> staging;
    staging.push_back(source);
    while (!staging.empty())
    {
        File srcfile = staging.front();
        staging.pop_front();
        if (sources_.get(srcfile))
            //This source file is already used
            continue;
        auto src = sources_.add(srcfile);

        Headers headers;
        MSS(src->searchForHeaders(headers));

        for (auto hdr: headers)
            MSS(headers_.add(hdr));
    }

    MSS_END();
}
