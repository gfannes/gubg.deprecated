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
	string output_files_(const CreateJob &job)
	{
		ostringstream oss;
		for (auto f: job.files)
			oss << f << " ";
		return oss.str();
	}
	ReturnCode CreateMgr::create(const CreateJob &job)
	{
		MSS_BEGIN(ReturnCode, output_files_(job));

		std::map<gubg::file::File, gubg::file::File> hfPerFile;
		bool all_found_in_cache = false;
		if (cacheExists_())
		{
			all_found_in_cache = true;
			for (auto f: job.files)
			{
				using namespace gubg::hash::md5;
				Hash hash;
				{
					Stream md5;
					md5 << job.command;
					md5 << f.name();
					hash = md5.hash();
					L("before: " << hash.to_hex());
					L("depend: " << job.dependencies.to_hex());
					hash ^= job.dependencies;
					L("after:  " << hash.to_hex());
				}
				auto ff = cache_; ff << hash.to_hex();
				hfPerFile[f] = ff;
				if (!gubg::file::exists(ff))
				{
					L("Could not find " << f << " in the cache (" << ff << ")");
					all_found_in_cache = false;
				}
			} 
		}

		if (!all_found_in_cache)
			MSS(execute_(job));

		if (cacheExists_())
		{
			for (auto p: hfPerFile)
			{
				if (all_found_in_cache)
					gubg::file::copy(p.second, p.first);
				else
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
