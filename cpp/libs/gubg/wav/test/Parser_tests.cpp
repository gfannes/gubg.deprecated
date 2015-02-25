#include "catch/catch.hpp"
#include "gubg/wav/Parser.hpp"
#include "gubg/log/HR.hpp"
using namespace std;
using namespace gubg;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace  { 
    class Parser: public wav::Parser_crtp<Parser>
    {
        public:
            size_t expected_sample_count = 0;
            size_t sample_count = 0;
            wav::Format fmt;

            void wav_begin() { S(); }
            void wav_format(const wav::Format &f)
            {
                SS(fmt);
                fmt = f;
            }
            void wav_begin_samples(size_t count)
            {
                SS(count);
                expected_sample_count = count;
            }
            template <typename T>
            void wav_sample(const std::vector<T> &sample)
            {
                SS_(log::hr(sample));
                ++sample_count;
            }
            void wav_end_samples()
            {
                SS(expected_sample_count, sample_count);
            }
            void wav_end() { S(); }
    };
} 

TEST_CASE("WAV parser test", "[wav]")
{
    Parser parser;
    REQUIRE(MSS_IS_OK(parser(file::File("piano2.wav"))));
    REQUIRE(parser.expected_sample_count == parser.sample_count);
}
#include "gubg/log/end.hpp"
