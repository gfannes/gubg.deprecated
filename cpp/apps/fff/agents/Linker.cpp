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

        board.addItem(hash_tag, cpp_object);
        board.addItem(hash_tag, c_object);
        if (board.isDirty())
            MSS_RETURN_OK();

        if (!run_())
            MSS_RETURN_OK();

        auto tvs = board.getFrom(0);

        linker::Vendor vendor = linker::Vendor::GCC;
        linker::OutputType exeType = linker::OutputType::Exe;
        for (auto tv: tvs)
        {
            if (tv.tag == start)
            {
                if (false) {}
                else if (tv.value == "cl")
                    vendor = linker::Vendor::MSC;
                else if (tv.value == "clang")
                    vendor = linker::Vendor::CLang;
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
            else if (tv.tag == cache_dir)
            {
                create_mgr.setCache(tv.value);
            }
            else if (tv.tag == start)
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
            else if (tv.tag == cpp_object || tv.tag == c_object)
            {
                lnk.addObject(tv.value);
                for (auto p: board.getRecursiveDependencies(tv))
                    rdeps[p.first] = p.second;
            }
            else if (tv.tag == cpp_include)
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
            else if (tv.tag == cpp_library_path)
                lnk.addLibraryPath(tv.value);
            else if (tv.tag == cpp_library)
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
                board.addItem(cpp_executable, executable);
                break;
            case linker::OutputType::Shared:
                board.addItem(cpp_shared_object, executable);
                break;
        }

        MSS_END();
    }
} }
#include "gubg/log/end.hpp"
