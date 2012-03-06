#ifndef gubg_coding_d9_codes_hpp
#define gubg_coding_d9_codes_hpp

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
