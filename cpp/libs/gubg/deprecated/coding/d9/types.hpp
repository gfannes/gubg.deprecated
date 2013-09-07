#ifndef HEADER_gubg_coding_d9_types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_coding_d9_types_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            typedef unsigned char ubyte;

            enum class Meta {Checksum, Version, Content, Source, Destination, PackageId, SequenceNr, SequenceTotal};
            enum class Format {Unknown = -1, AsIs, Block, Stream};
            std::string to_s(Format);
            enum class ContentType {NoContent = -1, Raw = 0, String_c, UNumber_be, UNumber_le, SNumber_be, SNumber_le, MsgpackMap, MsgpackArray};
            typedef unsigned long Address;
            typedef unsigned long Id;
            typedef std::vector<unsigned long> Ixs;

            struct Byte
            {
                const static ubyte D8 = 0xd8;
                const static ubyte D9 = 0xd9;
                const static ubyte End = 0xff;
            };
        }
    }
}

#endif
