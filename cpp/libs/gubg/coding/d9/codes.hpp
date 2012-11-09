#ifndef HEADER_gubg_coding_d9_codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_coding_d9_codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg
{
    namespace coding
    {
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
                SourceIsEmpty,
            };
        }
    }
}

#endif
