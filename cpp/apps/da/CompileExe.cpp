#include "da/CompileExe.hpp"
#include "da/build/Builder.hpp"
#include "da/build/Configuration.hpp"
#include "da/compile/Compiler.hpp"
#include "da/link/Linker.hpp"
using namespace da;
using namespace da::compile;
using namespace std;

CompileExe::CompileExe(const string &source):
    source_(source){}

ReturnCode CompileExe::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode);

    const Configuration configuration;
    Builder builder(configuration);
    MSS(builder.process(source_));

    Compiler compiler;
    {
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
        gubg::file::File object(source);
        object.setExtension("obj");
        MSS(compiler(object, source));
    }

    Linker linker;
    gubg::file::File executable(source_);
    executable.setExtension("exe");
    MSS(linker(executable, compiler.objectFiles()));

    MSS_END();
}
