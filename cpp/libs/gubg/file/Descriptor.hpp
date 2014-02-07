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
        const char *to_hr(EventType);

        class Select;

        class Descriptor
        {
            private:
                friend class Select;
                struct Pimpl;
                typedef std::shared_ptr<Pimpl> PP;

            public:
                static const int InvalidDesc = -1;

                size_t id() const;

                Descriptor(){}
                Descriptor(PP pp): pimpl_(pp) {}

                static Descriptor listen(unsigned short port, const std::string &ip = "");
                static Descriptor listen(File, AccessMode);

                static Descriptor std_in();

                ReturnCode accept(Descriptor &);

                static Descriptor connect(const std::string &ip, unsigned short port);

                ReturnCode read(std::string &buffer);
                ReturnCode write(size_t &written, const std::string &buffer);

                ReturnCode setBaudRate(int rate);

                void reset(){pimpl_.reset();}
                bool valid() const;

                bool operator<(const Descriptor &rhs) const;
                bool operator==(const Descriptor &rhs) const;

                void stream(std::ostream &) const;

            private:
                PP pimpl_;
        };

        class Select
        {
            public:
                ReturnCode add(Descriptor, AccessMode);
                ReturnCode erase(Descriptor, AccessMode);

                //Calls select() on the added Descriptors
                //Waits infinitely for a descriptor event
                ReturnCode process() { return process_(0); }
                //Waits for the specified timeout, 0 is polling
                ReturnCode process(std::chrono::milliseconds timeout) { return process_(&timeout); }
                //Waits for the specified timeout, 0 pointer waits infinitely
                ReturnCode process(const std::chrono::milliseconds *timeout);

                //Callbacks with typical select-events: a descriptor can be served
                virtual void select_ready(Descriptor, EventType) = 0;

            private:
                //This version of process_ is too dangerous to make public, timeout will be changed
                ReturnCode process_(std::chrono::milliseconds *timeout);
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
