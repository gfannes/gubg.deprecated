#include "da/CompileExe.hpp"
#include "da/build/Builder.hpp"
#include "da/compile/Compiler.hpp"
#include "da/link/Linker.hpp"
#include "da/FileCache.hpp"
using namespace da;
using namespace da::compile;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "CompileExe"
#include "gubg/log/begin.hpp"
CompileExe::CompileExe(const string &source, ExeType exeType):
    source_(source), exeType_(exeType)
{
}

da::ReturnCode CompileExe::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode);

    Builder builder;
    //Detect all header and source dependencies starting from source_
    MSS(builder.process(source_));

    //Setup the compiler
    Compiler compiler(exeType_);
    compiler.setCache(FileCache().dir());
    builder.extractCompileSettings(compiler.settings);

    //Compile all source files into object files
    for (auto source: builder.sources())
    {
        gubg::file::File object(source->file());
        object.setExtension("o");
        MSS(compiler(object, source->file(), builder.headers(source)));
    }
    MSS(compiler.nrFailures() == 0);

    //Setup the linker
    Linker linker(exeType_);
    builder.extractLinkSettings(linker.settings);

    //Link the object files into an exe
    gubg::file::File executable(source_);
    executable.setExtension("exe");
    MSS(linker(executable, compiler.objectFiles()));

    MSS_END();
}
