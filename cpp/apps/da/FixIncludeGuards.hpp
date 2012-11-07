#ifndef da_IncludeGuards_hpp
#define da_IncludeGuards_hpp

#include "da/Tasks.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/l.hpp"

namespace 
{
    struct Recursor: gubg::file::Recursor_crtp<Recursor>
    {
        typedef da::ReturnCode ReturnCode;

        template <typename File>
        ReturnCode recursor_discoveredFile(File &file)
        {
            MSS_BEGIN(ReturnCode);
            L(file.name());
            MSS_END();
        }
    };
}

namespace da
{
    class FixIncludeGuards: public ITask
    {
        public:
            static Ptr create(){return Ptr(new FixIncludeGuards);}
            virtual ReturnCode execute()
            {
                MSS_BEGIN(ReturnCode);
                Recursor()(gubg::file::getcwd());
                MSS_END();
            }
        private:
    };
}

#endif
