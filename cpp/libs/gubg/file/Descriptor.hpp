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
        enum class EventType {Read, Write, Open, Close};

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

                ReturnCode read(std::string &buffer);

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
                ReturnCode erase(Descriptor);

                //Calls select() on the added Descriptors
                //Waits infinitely for a descriptor event
                ReturnCode operator()() { return callOperator_(0); }
                //Waits for the specified timeout, 0 is polling
                ReturnCode operator()(std::chrono::milliseconds timeout) { return callOperator_(&timeout); }
                //Waits for the specified timeout, 0 pointer waits infinitely
                ReturnCode operator()(const std::chrono::milliseconds *timeout);

                //Callbacks with typical select-events: a descriptor can be served
                virtual bool select_ready(Descriptor, EventType) = 0;

            private:
                //This version of the call operator is too dangerous to make public, timeout will be changed
                ReturnCode callOperator_(std::chrono::milliseconds *timeout);
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
