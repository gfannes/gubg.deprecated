#include "da/build/Source.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#include <queue>
#include <set>
using namespace da;
using gubg::file::File;
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
                verbose("\tDetected header", str);
                staging.push(File(str));
            }
    };
}
ReturnCode Source::searchForHeaders(Headers &headers)
{
    MSS_BEGIN(ReturnCode);

    queue<File> staging;
    staging.push(file());
    set<File::Name> processed;

    IncludePusher includePusher(staging);

    while (!staging.empty())
    {
        auto f = staging.front();
        staging.pop();
        if (processed.count(f.name()))
            continue;
        verbose("Processing", f.name());
        processed.insert(f.name());

        includePusher.process(f);
    }

    MSS_END();
}
