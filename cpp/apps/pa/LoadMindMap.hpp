#ifndef HEADER_pa_LoadMindMap_hpp_ALREADY_INCLUDED
#define HEADER_pa_LoadMindMap_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class LoadMindMap: public ITask
    {
        public:
            static Ptr create(){return Ptr(new LoadMindMap());}

            virtual ReturnCode execute(const Options &);

        private:
            LoadMindMap(){}
    };
}

#endif
