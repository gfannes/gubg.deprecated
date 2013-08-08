#ifndef HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/file/Codes.hpp"
#include <memory>

namespace gubg
{
    namespace file
    {
        class Descriptor
        {
            public:
                static Descriptor listen(unsigned short port, const std::string &ip = "");
                static Descriptor open(File);
                void reset(){pimpl_.reset();}
                ReturnCode accept(Descriptor &);
            private:
                struct Pimpl;
                std::shared_ptr<Pimpl> pimpl_;
        };
    }
}

#endif
