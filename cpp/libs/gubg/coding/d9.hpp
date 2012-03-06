#ifndef gubg_coding_d9_hpp
#define gubg_coding_d9_hpp

//The d9-coding can be used to encode a binary buffer for serial transport. It features:
// * Space-efficiency, especially if 0xd9 and 0xd8 occur infrequent
//    * 100 bytes, no 0xd9 nor 0xd8 => 105 in stream format, 105 bytes in block format (+ optional checksum)
//    * 100 bytes, all 0xd9 or 0xd8 => 119 in stream format, 118 bytes in block format (+ optional checksum)
// * Performance-efficiency
// * Stream-format, allowing encoding/decoding as data arrives
// * Block-format, does not require memmoves
// * Both the start and end of an encoded message can be found unambiguously in a buffer of messages
// * Won't clash with current msgpack type identifiers
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
//    * Bit 6: sequence number attribute is present (for a partial package)
//    * Bit 7: total number attribute is present (for a partial package)
//
// * Run-length encoded number/bits: (0bbbbbbb)* 10bbbbbb
//    * Big-endian byte-order
// * Run-length encoded pair: (0fffssss)* 10ffssss
//    * Big-endian byte-order
//    * 'f'-bits are for the first number of the pair
//    * 's'-bits are for the second number of the pair

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
                MissingStart, MissingEnd, MissingFormat, TooFewAlterations, RLETooSmall,
                RLETooLarge, RLEIllegalMSBits, RLEClosingByteExpected, UnknownFormat, UnsupportedVersion,
                AlterationsNotAllowedForAsIs, PackageTooSmall, NoContentPresent, UnexpectedBitsEnd, CannotDecodeMeta,
                CannotDecodeContentMeta, UnknownMetaField, CannotDecodeSource, CannotDecodeDestination, CannotDecodeId,
                CannotReadCodedContent, ExpectedChecksum, ExpectedEndByte, IllegalEscapeSequence, IllegalFormat, 
            };

            enum class Meta {Checksum, Version, Content, Source, Destination, PackageId, SequenceNr, SequenceTotal};
            enum class Format {Unknown = -1, AsIs, Block, Stream};
            string to_s(Format);
            enum class ContentType {NoContent = -1, Raw = 0, String_c, UNumber_be, UNumber_le, SNumber_be, SNumber_le, MsgpackMap, MsgpackArray};
            typedef unsigned long Address;
            typedef unsigned long Id;
            typedef vector<unsigned long> Ixs;

            //Runlength encoding/decoding
            namespace rle
            {
                ReturnCode readRLE(string &coded, istream &is);
                string     encodeNumber(unsigned long);
                ReturnCode decodeNumber(unsigned long &number, const string &coded);
                ReturnCode decodeNumber(unsigned long &number, istream &coded);
                template <typename Number>
                    ReturnCode decodeNumber(Number &number, const string &coded)
                    {
                        MSS_BEGIN(ReturnCode);
                        unsigned long v;
                        MSS(decodeNumber(v, coded));
                        number = v;
                        MSS_END();
                    }
                template <typename Number>
                    ReturnCode decodeNumber(Number &number, istream &coded)
                    {
                        MSS_BEGIN(ReturnCode);
                        unsigned long v;
                        MSS(decodeNumber(v, coded));
                        number = v;
                        MSS_END();
                    }
                string     encodePair(unsigned long first, unsigned long second);
                ReturnCode decodePair(unsigned long &first, unsigned long &second, const string &coded);
                ReturnCode decodePair(unsigned long &first, unsigned long &second, istream &coded);
                template <typename Base>
                    string encodePair(pair<Base, Base> p){return encodePair(p.first, p.second);}
                template <typename Base>
                    ReturnCode decodePair(pair<Base, Base> &p, const string &coded){return decodePair(p.first, p.second, coded);}

                class Bits
                {
                    public:
                        Bits();
                        void add(bool);
                        string encode() const;
                        //Returns the ixs where a 1 is found
                        static ReturnCode decode(Ixs &ixs, const string &coded);
                        static ReturnCode decode(Ixs &ixs, istream &coded);
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

                    //Getters
                    ReturnCode getContent(string &plain) const;

                    ReturnCode encode(string &coded) const;
                    ReturnCode decode(const string &coded);
                    ReturnCode decode(istream &is);

                private:
                    void clear_();

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
