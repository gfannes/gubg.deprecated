#include "fff/tools/Linker.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
#include "fff/Linker.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Linker"
#include "gubg/log/begin.hpp"
namespace fff { namespace tools { 

    ReturnCode Linker::process(Board &board)
    {
        MSS_BEGIN(ReturnCode);

        if (!run_())
            MSS_RETURN_OK();

        auto tvs = board.getFrom(0);

        linker::Vendor vendor = linker::Vendor::GCC;
        linker::OutputType exeType = linker::OutputType::Exe;
        for (auto tv: tvs)
        {
            if (tv.first == Tag("start"))
            {
                if (false) {}
                else if (tv.second.string() == "cl")
                    vendor = linker::Vendor::MSC;
                else if (tv.second.string() == "shared")
                    exeType = linker::OutputType::Shared;
            }
        }
        fff::Linker lnk(vendor, exeType);

        file::File executable;
        OnlyOnce setExecutable;
        CreateMgr create_mgr;
        Dependencies dependencies;
        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.first == Tag("cache"))
            {
                create_mgr.setCache(tv.second.file());
            }
            else if (tv.first == Tag("c++", "source"))
            {
                if (setExecutable())
                    executable = tv.second.file();
            }
            else if (tv.first == Tag("c++", "object") || tv.first == Tag("c", "object"))
            {
                lnk.addObject(tv.second.file());
                dependencies.insert(tv);
            }
            else if (tv.first == Tag("c++", "include"))
            {
#ifdef GUBG_API_LINUX
                if (tv.second.string() == "dlfcn.h")
                    lnk.addLibrary("dl");
#endif
#ifdef GUBG_API_POSIX
                if (tv.second.string() == "thread")
                    lnk.addOption("thread");
#endif
            }
            else if (tv.first == Tag("c++", "library_path"))
                lnk.addLibraryPath(tv.second.file());
            else if (tv.first == Tag("c++", "library"))
                lnk.addLibrary(tv.second.string());
        }

        switch (exeType)
        {
            case linker::OutputType::Exe:
                executable.setExtension("exe");
                break;
            case linker::OutputType::Shared:
                executable.setExtension("dll");
                break;
        }

        CreateJob job;
        job.files.insert(executable);
        std::string cmd;
        lnk.link(cmd, executable);
        job.command = cmd;
        job.dependencies = board.hash(dependencies);
        MSS(create_mgr.create(job), LinkFailure);
        {
            using namespace gubg::file;
            MSS(file::chmod(executable, {All, Read, Read}));
        }

        switch (exeType)
        {
            case linker::OutputType::Exe:    board.add(Tag("c++", "executable"), executable); break;
            case linker::OutputType::Shared: board.add(Tag("c++", "shared_object"), executable); break;
        }

        MSS_END();
    }
} }
#include "gubg/log/end.hpp"
