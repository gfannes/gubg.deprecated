#include "fff/Create.hpp"
#include "fff/Execute.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace std;

#define GUBG_MODULE_ "Create"
#include "gubg/log/begin.hpp"
namespace fff { 
	ReturnCode CreateMgr::setCache(gubg::file::File cache)
	{
		MSS_BEGIN(ReturnCode);
		MSS(gubg::file::exists(cache));
		cache_ = cache;
		MSS_END();
	}
	ReturnCode CreateMgr::create(const CreateJob &job)
	{
		MSS_BEGIN(ReturnCode, job.file);

		std::map<gubg::file::File, gubg::file::File> hfPerFile;
		bool found_in_cache = false;
		if (cacheExists_())
        {
            found_in_cache = true;
            using namespace gubg::hash::md5;
            Hash hash;
            {
                Stream md5;
                md5 << job.command;
                md5 << job.file.name();
                hash = md5.hash();
                L("before: " << hash.to_hex());
                L("depend: " << job.dependencies.to_hex());
                hash ^= job.dependencies;
                L("after:  " << hash.to_hex());
            }
            auto ff = cache_; ff << hash.to_hex();
            hfPerFile[job.file] = ff;
            if (!gubg::file::exists(ff))
            {
                L("Could not find " << job.file << " in the cache (" << ff << ")");
                found_in_cache = false;
            }
        }

		if (!found_in_cache)
        {
            //Remove the file before we will create it. This way, we can check that execution actually worked.
            gubg::file::remove(job.file);
			MSS(execute_(job));
            //Check that execute_ created the file
            MSS(gubg::file::exists(job.file));
        }

		if (cacheExists_())
		{
			for (auto p: hfPerFile)
			{
				if (found_in_cache)
                    //Copy from cache to here
					gubg::file::copy(p.second, p.first);
				else
                    //Copy into the cache
					gubg::file::copy(p.first, p.second);
			}
		}

		MSS_END();
	}
	ReturnCode CreateMgr::execute_(const CreateJob &job)
	{
		return execute(job.command);
	}
	bool CreateMgr::cacheExists_() const
	{
		return !cache_.empty();
	}
} 
#include "gubg/log/end.hpp"
