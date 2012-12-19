//#define GUBG_LOG
#include "da/CompileExe.hpp"
#include "da/build/Builder.hpp"
#include "da/build/Configuration.hpp"
#include "da/compile/Compiler.hpp"
#include "da/link/Linker.hpp"
using namespace da;
using namespace da::compile;
using namespace gubg::file;
using namespace std;

CompileExe::CompileExe(const string &source):
    source_(source)
{
}

da::ReturnCode CompileExe::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode, execute);

    const Configuration configuration;
    Builder builder(configuration);
    MSS(builder.process(source_));

    //Setup the compiler
    Compiler compiler;
    {
        compiler.setCache(File("/home/gfannes/tmp/da"));
        const auto &config = configuration.compiler;
        for (auto def: config.defines)
            compiler.addDefine(def);
        for (auto setting: config.settings)
            compiler.addSetting(setting);
        for (auto path: config.includePaths)
            compiler.addIncludePath(path);
    }

    for (auto source: builder.sources())
    {
        gubg::file::File object(source->file());
        object.setExtension("o");
        MSS(compiler(object, source->file(), builder.headers(source)));
    }

    Linker linker;
    {
        const auto &config = configuration.linker;
        for (auto setting: config.settings)
            linker.addSetting(setting);
        for (auto lib: config.libraries)
            linker.addLibrary(lib);
        for (auto path: config.libraryPaths)
            linker.addLibraryPath(path);
    }
    gubg::file::File executable(source_);
    executable.setExtension("exe");
    MSS(linker(executable, compiler.objectFiles()));

    MSS_END();
}
