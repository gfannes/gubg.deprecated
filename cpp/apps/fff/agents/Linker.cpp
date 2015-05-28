#include "fff/agents/Linker.hpp"
#include "fff/Create.hpp"
#include "fff/Linker.hpp"
#include "fff/Tags.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Linker"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

    gubg::bbs::ReturnCode Linker::process(gubg::bbs::Board &board)
    {
        MSS_BEGIN(gubg::bbs::ReturnCode);

        if (addHashTags_())
        {
            board.addItem(hash_tag, "c++.object");
            board.addItem(hash_tag, "c.object");
            MSS_RETURN_OK();
        }

        if (!run_())
            MSS_RETURN_OK();

        auto tvs = board.getFrom(0);

        linker::Vendor vendor = linker::Vendor::GCC;
        linker::OutputType exeType = linker::OutputType::Exe;
        for (auto tv: tvs)
        {
            if (tv.tag == "start")
            {
                if (false) {}
                else if (tv.value == "cl")
                    vendor = linker::Vendor::MSC;
                else if (tv.value == "shared")
                    exeType = linker::OutputType::Shared;
            }
        }
        fff::Linker lnk(vendor, exeType);

        file::File executable;
        OnlyOnce setExecutable;
        CreateMgr create_mgr;
        gubg::bbs::RecursiveDependencies rdeps;
        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == "cache")
            {
                create_mgr.setCache(tv.value);
            }
            else if (tv.tag == "start")
            {
                if (false) {}
                else if (tv.value == "debug")
                    lnk.addOption("debug");
            }
            else if (tv.tag == cpp_source)
            {
                if (setExecutable())
                    executable = tv.value;
            }
            else if (tv.tag == "c++.object" || tv.tag == "c.object")
            {
                lnk.addObject(tv.value);
                for (auto p: board.getRecursiveDependencies(tv))
                    rdeps[p.first] = p.second;
            }
            else if (tv.tag == "c++.include")
            {
#ifdef GUBG_API_LINUX
                if (tv.value == "dlfcn.h")
                    lnk.addLibrary("dl");
#endif
#ifdef GUBG_API_POSIX
                if (tv.value == "thread")
                    lnk.addOption("thread");
#endif
            }
            else if (tv.tag == "c++.library_path")
                lnk.addLibraryPath(tv.value);
            else if (tv.tag == "c++.library")
                lnk.addLibrary(tv.value);
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
        job.file = executable;
        std::string cmd;
        lnk.link(cmd, executable);
        job.command = cmd;
        job.dependencies = rdeps;
        MSS(create_mgr.create(job));//LinkFailure
        {
            using namespace gubg::file;
            MSS(file::chmod(executable, {All, Read, Read}));
        }

        switch (exeType)
        {
            case linker::OutputType::Exe:
                board.addItem("c++.executable", executable);
                break;
            case linker::OutputType::Shared:
                board.addItem("c++.shared_object", executable);
                break;
        }

        MSS_END();
    }
} }
#include "gubg/log/end.hpp"
