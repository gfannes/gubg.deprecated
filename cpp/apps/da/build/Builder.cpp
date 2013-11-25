#include "da/build/Builder.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include "gubg/string_algo.hpp"
#include <queue>
using namespace da;
using namespace da::package;
using namespace gubg;
using namespace gubg::file;
using gubg::file::File;
using namespace std;

#define GUBG_MODULE "Builder"
#include "gubg/log/begin.hpp"
Builder::Builder()
{
    {
        string str;
        if (env::expand(str, "$GUBG"))
            packages_ << GUBG::create(File(str));
#ifdef GUBG_LINUX
        if (env::expand(str, "$GUBG_BOOST"))
            packages_ << Boost::create(File(str));
#endif
#ifdef GUBG_MINGW
        packages_ << Boost::create(File(str));
#endif
        if (env::expand(str, "$GUBG_NANA"))
            packages_ << Nana::create(File(str));
        if (env::expand(str, "$GUBG_ARDUINO"))
            packages_ << Arduino::create(File(str));
        if (env::expand(str, "$GUBG_DECODEIT"))
            packages_ << DecodeIt::create(File(str));
    }

    packages_ << Local::create(File(getcwd()));

    packages_ << SDL::create(File(""));
    packages_ << Libs::create(File(""));

	packages_ << (forest_ = package::Forest::create());

    packages_.prune();

    LL("I found following packages: " << (string_algo::join(packages_.names(), ", ")));
}

da::ReturnCode Builder::process(const SourceFile &source)
{
    MSS_BEGIN(ReturnCode);

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
        SourceFiles sisterFiles;
        MSS(src->searchForHeaders(headers, sisterFiles, packages_));
        headersPerSource_[src] = headers;
        packages_.extractCompileSettings(compileSettings_);

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

da::ReturnCode Builder::addTree(gubg::file::File tree)
{
	MSS_BEGIN(ReturnCode);
    MSS(resolve(tree));
	MSS(!tree.empty());
	forest_->addTree(tree);
	MSS_END();
}

da::ReturnCode Builder::addInclude(const gubg::file::File &inc)
{
	MSS_BEGIN(ReturnCode);
	MSS(!inc.empty());
    compileSettings_.includePaths.insert(inc);
	MSS_END();
}

Headers Builder::headers(Source::Ptr src) const
{
    const auto it = headersPerSource_.find(src);
    if (it == headersPerSource_.end())
        return Headers();
    return it->second;
}
