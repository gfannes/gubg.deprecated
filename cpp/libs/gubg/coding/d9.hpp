#ifndef gubg_coding_d9_hpp
#define gubg_coding_d9_hpp

//The d9-coding can be used to encode a binary buffer for serial transport. It features:
// * Space-efficiency, especially if 0xd9 and 0xd8 do not occur a lot (e.g., won't clash with current msgpack type identifiers)
//    * 100 bytes, no 0xd9 nor 0xd8 => 105 in stream mode, 106 bytes in block mode
//    * 100 bytes, all 0xd9 or 0xd8 => 119 in stream mode, 118 bytes in block mode
// * Performance-efficiency
// * Stream-mode, allowing encoding/decoding as data arrives
// * Block-mode, does not require memmoves
// * Both the start and end of an encoded message can be found unambiguously in a buffer of messages
//
//The general package format looks like:
//
//                --package-start--   alterations         ----------0xd9-free-buffer--------------- ---package-end---
//                0xd9     0xd9       |||||||   0xff                                               0xd9     0xd8
// * Block mode:  11011001 11011001 (0???????)* 11111111  <0xd9-free data, same size as orig data>  11011001 11011000
// * Stream mode: 11011001 11011001             10000000 (<0xd8/9-free data> | 11011000 ###?????)* 11011001 11011000
//                                              0x80
//
// * Package start     : <0xd9 0xd9>
// * Block-mode        : All alterations are given upfront and ended with a 0xff. There should not be more 0xd8
//                       occurences in the 0xd9-free buffer than alterations specified
// * Stream-mode       : Each occurence of (0xd8 ###?????) corresponds with an escaped sequence of at most 5 0xd8 or 0xd9 bytes
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
        namespace d9
        {
            enum class ReturnCode
            {
                MSS_DEFAULT_CODES,
                MissingStart, MissingEnd, MissingMode,
                TooFewAlterations,
            };

            enum Mode {Block, Stream};

            //Encodes plain into d9 using the specified mode
            string encode(const string &plain, Mode);

            //Decodes a d9-coded buffer into plain, optionally returning the mode that was used for coded
            ReturnCode decode(string &plain, const string &coded, Mode *mode = 0);
        }
    }
}

#endif
