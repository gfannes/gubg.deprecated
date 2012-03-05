#ifndef gubg_coding_d9_hpp
#define gubg_coding_d9_hpp

//The d9-coding can be used to encode a binary buffer for serial transport. It features:
// * Space-efficiency, especially if 0xd9 and 0xd8 do not occur a lot (e.g., won't clash with current msgpack type identifiers)
//    * 100 bytes, no 0xd9 nor 0xd8 => 105 in stream format, 106 bytes in block format
//    * 100 bytes, all 0xd9 or 0xd8 => 119 in stream format, 118 bytes in block format
// * Performance-efficiency
// * Stream-format, allowing encoding/decoding as data arrives
// * Block-format, does not require memmoves
// * Both the start and end of an encoded message can be found unambiguously in a buffer of messages
//
//The general package format looks like:
//
//            0xd9     metabyte attributes    content                                      checksum  0xd9     0xff
// * Package: 11011001 0bbbbbbb (rle-number)* (<0xd9-free data, stream or block encoded>)? 0bbbbbbb (11011001 11111111)?
// * Meta-bits for version 0:
//    * Bit 0: 7-bit checksum is added, either at the end, or before the close 0xd9, 0xff pair
//    * Bit 1: version
//    * Bit 2: package has content and content format/type pair attribute is given
//    * Bit 3: source attribute is present (default: unknown)
//    * Bit 4: destination attribute is present (default: everybody)
//    * Bit 5: package-id attribute is present
// * Run-length encoded number: (0bbbbbbb)* 10bbbbbb
//    * Big-endian byte-order
//
// * Extended attributes:
//    * Bit 0: sequence number for a partial package
//    * Bit 1: total number for a partial package
//
//DEPRECATED:
//                --package-start--   alterations         ----------0xd9-free-buffer--------------- ---package-end---
//                0xd9     0xd9       |||||||   0xff                                               0xd9     0xd8
// * Block format:  11011001 11011001 (0???????)* 11111111  <0xd9-free data, same size as orig data>  11011001 11011000
// * Stream format: 11011001 11011001             10000000 (<0xd8/9-free data> | 11011000 ###?????)* 11011001 11011000
//                                              0x80
//
// * Package start     : <0xd9 0xd9>
// * Block-format        : All alterations are given upfront and ended with a 0xff. There should not be more 0xd8
//                       occurences in the 0xd9-free buffer than alterations specified
// * Stream-format       : Each occurence of (0xd8 ###?????) corresponds with an escaped sequence of at most 5 0xd8 or 0xd9 bytes
//                       ### indicate how many 0xd8/9 bytes the escape sequence should be expanded to
//                       ????? indicate the alterations: 0 => 0xd8, 1 => 0xd9, from least to most significant
// * Alteration bit (?): 0 => 0xd8, 1 => 0xd9
//                       Excess alterations should be set to 0
// * Package end       : <0xd9 0xd8>

#include "gubg/mss.hpp"
#include <string>

namespace gubg
{
    using namespace std;
    namespace coding
    {
        typedef unsigned char ubyte;
        namespace d9
        {
            enum class ReturnCode
            {
                MSS_DEFAULT_CODES,
                MissingStart, MissingEnd, MissingFormat,
                TooFewAlterations,
                RLETooSmall, RLETooLarge, RLEIllegaleMSBits, RLEClosingByteExpected,
                UnknownFormat,
                UnsupportedVersion,
                AlterationsNotAllowedForAsIs,
            };

            enum class Meta {Checksum, Version, Content, Source, Destination, PackageId};
            enum class Format {Unknown = -1, AsIs, Block, Stream};
            string to_s(Format);
            enum class ContentType {NoContent = -1, Raw = 0, String_c, UNumber_be, UNumber_le, SNumber_be, SNumber_le, MsgpackMap, MsgpackArray};
            typedef unsigned long Address;
            typedef unsigned long Id;

            //Runlength encoding/decoding
            namespace rle
            {
                string     encodeNumber(unsigned long);
                ReturnCode decodeNumber(unsigned long &number, const string &coded);
                string     encodePair(unsigned long first, unsigned long second);
                ReturnCode decodePair(unsigned long &first, unsigned long &second, const string &coded);
                template <typename Base>
                    string encodePair(pair<Base, Base> p){return encodePair(p.first, p.second);}
                template <typename Base>
                    ReturnCode decodePair(pair<Base, Base> &p, const string &coded){return decodePair(p.first, p.second, coded);}

                class Bits
                {
                    public:
                        Bits();
                        void add(bool);
                        string coded() const;
                        void clear();
                        bool empty() const;
                    private:
                        void appendCurrentToBuffer_();

                        ubyte current;
                        ubyte nr;
                        string buffer;
                };
            }

            //Represents a d9 package:
            // * Can be setup from a d9 message
            // * Can generate itself in d9
            class Package
            {
                public:
                    Package();

                    //Setters, allowing chained setup
                    Package &checksum(bool);
                    Package &format(Format);
                    Package &content(string plain, ContentType ct = ContentType::Raw);
                    Package &content(string &&plain, ContentType ct = ContentType::Raw);
                    Package &source(Address);
                    Package &destination(Address);
                    Package &id(Id);

                    ReturnCode encode(string &coded) const;
                    ReturnCode decode(string &plain) const;

                private:
                    bool checksum_;
                    size_t version_;
                    Format format_;
                    ContentType contentType_;
                    string content_;
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
