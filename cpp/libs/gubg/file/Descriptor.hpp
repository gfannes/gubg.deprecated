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
                struct Pimpl;
                typedef std::shared_ptr<Pimpl> PP;

            public:
                static const int InvalidDesc = -1;

                Descriptor(){}
                Descriptor(PP pp): pimpl_(pp) {}

                static Descriptor listen(unsigned short port, const std::string &ip = "");
                static Descriptor listen(File, AccessMode);

                ReturnCode accept(Descriptor &);

                void reset(){pimpl_.reset();}
                bool valid() const;

                bool operator<(const Descriptor &rhs) const;

                void stream(std::ostream &) const;

            private:
                PP pimpl_;
        };

        class Select
        {
            public:
                ReturnCode add(Descriptor, AccessMode);

                ReturnCode operator()(std::chrono::milliseconds timeout);

                virtual void select_readyToRead(Descriptor) = 0;
                virtual void select_readyToWrite(Descriptor) = 0;

            private:
                bool invariants_() const;

                typedef Descriptor::PP PP;
                typedef std::set<PP> Set;
                Set read_set_;
                Set write_set_;
        };
    }
}

namespace std
{
    inline ostream &operator<<(ostream &os, const gubg::file::Descriptor &d)
    {
        d.stream(os);
        return os;
    }
}

#endif
