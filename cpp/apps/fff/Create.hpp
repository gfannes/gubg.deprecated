#ifndef HEADER_create_ALREADY_INCLUDED
#define HEADER_create_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include "fff/Types.hpp"
#include "gubg/file/File.hpp"
#include <string>

namespace fff { 
    struct CreateJob
    {
        typedef std::set<gubg::file::File> Files;
        Files files;
        std::string command;
        Dependencies dependencies;
    };

    class CreateMgr
    {
        public:
            CreateMgr(gubg::file::File cache): cache_(cache) {}

            ReturnCode create(const CreateJob &);

        private:
            gubg::file::File cache_;
    };
} 

#endif
