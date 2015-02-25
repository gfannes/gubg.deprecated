#ifndef HEADER_gubg_wav_Format_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_Format_hpp_ALREADY_INCLUDED

#include <cstdint>
#include <ostream>

namespace gubg { namespace wav { 

    struct Format
    {
        std::uint16_t format = 0;
        std::uint16_t channel_count = 0;
        std::uint32_t sample_rate = 0;
        std::uint32_t byte_rate = 0;
        std::uint16_t block_size = 0;
        std::uint16_t bits_per_sample = 0;
    };

    inline std::ostream &operator<<(std::ostream &os, const Format &fmt)
    {
        os << "format: " << fmt.format << ", channel_count: " << fmt.channel_count << ", sample_rate: " << fmt.sample_rate << ", byte_rate: " << fmt.byte_rate << ", block_size: " << fmt.block_size << ", bits_per_sample: " << fmt.bits_per_sample;
        return os;
    }

} } 

#endif
