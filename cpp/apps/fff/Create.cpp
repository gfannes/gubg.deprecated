#include "fff/Create.hpp"
#include "fff/Execute.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace std;

#define GUBG_MODULE_ "Create"
#include "gubg/log/begin.hpp"
namespace fff { 
    ReturnCode CreateMgr::create(const CreateJob &job)
    {
        MSS_BEGIN(ReturnCode);
        std::map<gubg::file::File, gubg::file::File> hfPerFile;
        bool all_found = true;
        for (auto f: job.files)
        {
            gubg::hash::MD5 md5;
            md5 << job.command;
            md5 << f.name();
            auto ff = cache_; ff << md5.hash_hex();
            hfPerFile[f] = ff;
            if (!gubg::file::exists(ff))
                all_found = false;
        } 
        if (!all_found)
            MSS(execute(job.command));
        for (auto p: hfPerFile)
        {
            if (all_found)
                gubg::file::copy(p.second, p.first);
            else
                gubg::file::copy(p.first, p.second);
        }
        MSS_END();
    }
} 
#include "gubg/log/end.hpp"
