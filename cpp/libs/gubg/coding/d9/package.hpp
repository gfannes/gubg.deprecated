#ifndef HEADER_gubg_coding_d9_package_hpp_ALREADY_INCLUDED
#define HEADER_gubg_coding_d9_package_hpp_ALREADY_INCLUDED

//Represents a d9 package:
// * Can be setup from a d9 message
// * Can generate itself in d9

#include "gubg/coding/d9/types.hpp"
#include "gubg/coding/d9/codes.hpp"
#include "gubg/coding/d9/io.hpp"
#include <string>

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            class Package
            {
                public:
                    Package();

                    //Setters, allowing chained setup
                    Package &checksum(bool);
                    Package &format(Format);
                    Package &content(std::string plain, ContentType ct = ContentType::Raw);
                    Package &content(std::string &&plain, ContentType ct = ContentType::Raw);
                    Package &source(Address);
                    Package &destination(Address);
                    Package &id(Id);

                    //Getters
                    ReturnCode getContent(std::string &plain) const;

                    ReturnCode encode(std::string &coded) const;
                    ReturnCode decode(const std::string &coded);
                    ReturnCode decode(IInput &);

                private:
                    void clear_();

                    bool checksum_;
                    size_t version_;
                    Format format_;
                    ContentType contentType_;
                    std::string content_;
                    //We hold the addresses in signed variables, but they are unsigned
                    //A negative address is used to indicate the absence of an address
                    long src_;
                    long dst_;

                    long id_;
            };
        }
    }
}

#endif
