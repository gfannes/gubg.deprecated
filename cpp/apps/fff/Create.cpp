#include "fff/Create.hpp"
#include "fff/Execute.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace std;
using namespace gubg;

#define GUBG_MODULE_ "Create"
#include "gubg/log/begin.hpp"
namespace fff { 
    ReturnCode CreateMgr::setCache(gubg::file::File cache)
    {
        MSS_BEGIN(ReturnCode);
        DB db{new gubg::db::KeyValue{cache}};
        MSS(db->valid());
        cache_db_.swap(db);
        MSS_END();
    }
    ReturnCode CreateMgr::create(const CreateJob &job)
    {
        MSS_BEGIN(ReturnCode, job.file);

        //Remove the file before we will create it. This way, we can check that execution actually worked.
        gubg::file::remove(job.file);

        db::Key deps;
        {
            ostringstream key;
            key << job.command << endl;
            key << job.file << endl;
            for (auto p: job.dependencies)
            {
                key << p.first << endl;
                for (auto dep: p.second)
                    key << "  " << dep << endl;
            }
            deps = key.str();
        }
        db::Value content;

        if (cache_db_ && MSS_IS_OK(cache_db_->get(content, deps)))
        {
            //Copy from cache to here
            gubg::file::write(content, job.file);
        }
        else
        {
            MSS(execute(job.command));

            //Copy into the cache
            if (cache_db_)
            {
                //Storing into the cache can fail, but reading the file should not.
                MSS(gubg::file::read(content, job.file));
                cache_db_->set(deps, content);
            }
        }

        //Double-check that the file is present
        MSS(gubg::file::isRegular(job.file));

        MSS_END();
    }
} 
#include "gubg/log/end.hpp"
