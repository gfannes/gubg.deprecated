#define GUBG_LOG
#include "da/build/Source.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#include <queue>
#include <set>
using namespace da;
using gubg::file::File;
using gubg::file::Forest;
using namespace std;

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
ReturnCode Source::searchForHeaders(Headers &headers, const Forest &forest)
{
    MSS_BEGIN(ReturnCode, searchForHeaders, file().name());

    queue<File> staging;
    IncludePusher includePusher(staging);
    verbose("Processing top-level", file().name());
    MSS(includePusher.process(file()));

    set<File::Name> processed;

    while (!staging.empty())
    {
        File hdr;
        {
            const auto f = staging.front();
            staging.pop();
            if (!gubg::mss::isOK(forest.resolve(hdr, f, 1)))
                continue;
        }

        if (processed.count(hdr.name()))
            continue;
        processed.insert(hdr.name());
        headers.add(hdr);

        MSS(includePusher.process(hdr));
    }

    MSS_END();
}
