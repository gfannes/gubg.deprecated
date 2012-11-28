#include "da/build/Builder.hpp"
using namespace da;

ReturnCode Builder::process(const SourceFile &source)
{
    MSS_BEGIN(ReturnCode);

    sourceFiles_.clear();
    sourceFiles_.push_back(source);

    MSS_END();
}

const Builder::SourceFiles &Builder::sources() const
{
    return sourceFiles_;
}
