//#define GUBG_LOG
#include "da/CompileExe.hpp"
#include "da/build/Builder.hpp"
#include "da/build/Configuration.hpp"
#include "da/compile/Compiler.hpp"
#include "da/link/Linker.hpp"
#include "gubg/file/Creater.hpp"
#include "gubg/Settings.hpp"
using namespace da;
using namespace da::compile;
using namespace gubg::file;
using namespace std;

CompileExe::CompileExe(const string &source):
    source_(source)
{
}

namespace
{
    enum class Key {Object, DependencyMD5, Command};
    typedef gubg::Settings<Key> Settings;
    class CachedCompiler: public Creater_crtp<CachedCompiler>
    {
        public:
            CachedCompiler()
            {
                setCache(File("/home/gfannes/tmp/da"));
            }

            template <typename Files>
                da::ReturnCode creater_create(const Files &files, const Settings &settings) const
                {
                    MSS_BEGIN(da::ReturnCode);
                    string cmd;
                    MSS(settings.get(cmd, Key::Command));
                    L(cmd);
                    MSS(::system(cmd.c_str()) == 0, CompilationFailed);
                    MSS_END();
                }
    };
}
da::ReturnCode CompileExe::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode, execute);

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
    CachedCompiler cc;
    for (auto source: builder.sources())
    {
        gubg::file::File object(source->file());
        object.setExtension("o");
        Settings settings;
        settings.set(Key::Object, object.name());
        {
            gubg::hash::MD5 md5;
            string content;
            MSS(read(content, source->file()));
            md5 << content;
            auto headers = builder.headers(source);
            for (auto h: headers)
            {
                if (configuration.verbose)
                    L(h->file().name());
                MSS(read(content, h->file()));
                md5 << content;
            }
            settings.set(Key::DependencyMD5, md5.hash_hex());
        }
        settings.set(Key::Command, compiler.command(object, source->file()));
        MSS(cc({object}, settings));
        compiler.addObject(object);
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
