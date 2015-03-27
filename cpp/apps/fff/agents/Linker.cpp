#include "fff/agents/Linker.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
#include "fff/Linker.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Linker"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

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
            if (tv.tag == Tag("start"))
            {
                if (false) {}
                else if (tv.value.as_string() == "cl")
                    vendor = linker::Vendor::MSC;
                else if (tv.value.as_string() == "shared")
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
            else if (tv.tag == Tag("cache"))
            {
                create_mgr.setCache(tv.value.as_file());
            }
            else if (tv.tag == Tag("start"))
            {
                if (false) {}
                else if (tv.value.as_string() == "debug")
                    lnk.addOption("debug");
            }
            else if (tv.tag == Tag("c++", "source"))
            {
                if (setExecutable())
                    executable = tv.value.as_file();
            }
            else if (tv.tag == Tag("c++", "object") || tv.tag == Tag("c", "object"))
            {
                lnk.addObject(tv.value.as_file());
                dependencies.insert(tv);
            }
            else if (tv.tag == Tag("c++", "include"))
            {
#ifdef GUBG_API_LINUX
                if (tv.value.as_string() == "dlfcn.h")
                    lnk.addLibrary("dl");
#endif
#ifdef GUBG_API_POSIX
                if (tv.value.as_string() == "thread")
                    lnk.addOption("thread");
#endif
            }
            else if (tv.tag == Tag("c++", "library_path"))
                lnk.addLibraryPath(tv.value.as_file());
            else if (tv.tag == Tag("c++", "library"))
                lnk.addLibrary(tv.value.as_string());
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
