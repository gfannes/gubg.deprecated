#ifndef gubg_coding_d9_d9_hpp
#define gubg_coding_d9_d9_hpp

//The d9-coding can be used to encode a binary buffer for serial transport. It features:
// * Space-efficiency, especially if 0xd9 and 0xd8 occur infrequent
//    * no content                       => 2 (without checksum)
//    * empty content                    => 5, both for stream and block (without checksum)
//    * 100 bytes, no 0xd9 nor 0xd8      => 105 in stream format, 105 bytes in block format (without checksum)
//    * 100 bytes, all 0xd9 or 0xd8      => 45 in stream format,  120 bytes in block format (without checksum)
//    * 100 bytes, alternating 0xd8, 'a' => 155 in stream format, 113 bytes in block format (without checksum)
// * Performance-efficiency
// * Stream-format, allowing encoding/decoding as data arrives
// * Block-format, does not require memmoves
// * Both the start and end of an encoded message can be found unambiguously in a buffer of messages
// * Content-type information is stored too
// * Won't clash with current msgpack type identifiers
// * Optional goodies like checksum, source/destination, package-id, partial packages
// * No fixed-width numbers used, everything is run-length encoded, hopefully this helps to last
// * Extensible
//
//The general package format looks like:
//
//            0xd9     metabyte attributes    content                                      0xd9     0xff        checksum
// * Package: 11011001 0bbbbbbb (rle-number)* (<0xd9-free data, stream or block encoded>)? (11011001 11111111)? (0bbbbbbb)?
// * Meta-bits for version 0:
//    * Bit 0: 7-bit checksum is added at the end
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
