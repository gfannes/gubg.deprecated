#ifndef HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED

#include <memory>

namespace gubg
{
    namespace file
    {
        class Descriptor
        {
            public:
                void reset(){pimpl_.reset();}
            private:
                struct Pimpl;
                std::shared_ptr<Pimpl> pimpl_;
        };
    }
}

#endif
