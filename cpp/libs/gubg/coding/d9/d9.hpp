#ifndef gubg_coding_d9_d9_hpp
#define gubg_coding_d9_d9_hpp

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

#include "gubg/coding/d9/package.hpp"
#include "gubg/coding/d9/parser.hpp"

#endif
