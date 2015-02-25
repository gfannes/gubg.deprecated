#include "catch/catch.hpp"
#include "gubg/wav/Format.hpp"
using namespace gubg;

TEST_CASE("WAV Format", "[wav]")
{
    wav::Format fmt;
    REQUIRE(0 == fmt.format);
    REQUIRE(0 == fmt.channel_count);
    REQUIRE(0 == fmt.sample_rate);
    REQUIRE(0 == fmt.byte_rate);
    REQUIRE(0 == fmt.block_size);
    REQUIRE(0 == fmt.bits_per_sample);
}
