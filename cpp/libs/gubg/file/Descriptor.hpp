#ifndef HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Descriptor_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/file/Codes.hpp"
#include <memory>
#include <set>
#include <chrono>

namespace gubg
{
    namespace file
    {
        enum class AccessMode {Read, Write, ReadWrite};

        class Select;

        class Descriptor
        {
            private:
                friend class Select;

            public:
                static const int InvalidDesc = -1;

                static Descriptor listen(unsigned short port, const std::string &ip = "");
                static Descriptor listen(File, AccessMode);

                ReturnCode accept(Descriptor &);

                void reset(){pimpl_.reset();}
                bool valid() const {return (bool)pimpl_;}

                bool operator<(const Descriptor &rhs) const;

                int desc() const;

            private:
                struct Pimpl;
                std::shared_ptr<Pimpl> pimpl_;
        };

        class Select
        {
            public:
                ReturnCode add(Descriptor, AccessMode);

                ReturnCode operator()(std::chrono::milliseconds timeout);

            protected:
                virtual void select_readyToRead(Descriptor) = 0;
                virtual void select_readyToWrite(Descriptor) = 0;

            private:
                typedef std::set<Descriptor> Set;
                Set read_set_;
                Set write_set_;
        };
    }
}

#endif
