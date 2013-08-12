#ifndef HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/file/Codes.hpp"
#include <memory>

namespace gubg
{
    namespace file
    {
        enum class AccessMode {Read, Write, ReadWrite};

        class Descriptor
        {
            public:
                static Descriptor listen(unsigned short port, const std::string &ip = "");
                static Descriptor listen(File, AccessMode);
                void reset(){pimpl_.reset();}
                bool valid() const {return (bool)pimpl_;}
                ReturnCode accept(Descriptor &);
            private:
                struct Pimpl;
                std::shared_ptr<Pimpl> pimpl_;
        };
    }
}

#endif
