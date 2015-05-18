#ifndef HEADER_fff_Create_hpp_ALREADY_INCLUDED
#define HEADER_fff_Create_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include "gubg/db/KeyValue.hpp"
#include "gubg/bbs/Board.hpp"
#include <string>
#include <memory>

namespace fff { 
    struct CreateJob
    {
        gubg::file::File file;
        std::string command;
        gubg::bbs::RecursiveDependencies dependencies;
    };

    class CreateMgr
    {
        public:
            ReturnCode setCache(gubg::file::File cache);

            ReturnCode create(const CreateJob &);

        private:
            using DB = std::unique_ptr<gubg::db::KeyValue>;
            DB cache_db_;
    };
} 

#endif
