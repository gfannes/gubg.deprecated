#include "da/package/Forest.hpp"
#include "da/Arduino.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include <string>
using namespace da::package;
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Forest"
#include "gubg/log/begin.hpp"
void Forest::addTree(const File &tree)
{
	S();L("Adding tree " << tree.name());
    forest_.add(tree, {"cpp", "hpp", "h"});
}

bool Forest::exists() const
{
    return true;
}

da::ReturnCode Forest::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode, partial.name());

    File root;
    MSS_Q(forest_.resolve(resolvedHeader, root, partial, 1), UnknownHeader);
	L("Resolved " << partial << " into " << resolvedHeader << " with root " << root);
	root.popBasename();
	compileSettings_.includePaths.insert(root);

    auto sisterFile = resolvedHeader;
    sisterFile.setExtension("cpp");
    L("Potential sisterfile: " << sisterFile.name());
    if (forest_.contains(sisterFile))
    {
        switch (compileSettings_.targetPlatform)
        {
            case Any:
            case Host:
            case Arduino:
                sisterFiles.insert(sisterFile);
                break;
        }
    }

    MSS_END();
}
#include "gubg/log/end.hpp"
