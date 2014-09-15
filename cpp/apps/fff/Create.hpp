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
		Hash dependencies;
    };

    class CreateMgr
    {
        public:
            ReturnCode setCache(gubg::file::File cache);

            ReturnCode create(const CreateJob &);

        private:
			ReturnCode execute_(const CreateJob &);
			bool cacheExists_() const;

            gubg::file::File cache_;
    };
} 

#endif
