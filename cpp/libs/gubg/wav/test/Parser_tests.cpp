#include "catch/catch.hpp"
#include "gubg/wav/Parser.hpp"
#include "gubg/log/HR.hpp"
#include "gubg/Plot.hpp"
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
            typedef vector<int16_t> Samples;
            Samples left, right;
            Samples::iterator left_it, right_it;

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
                left.resize(count/10);
                right.resize(count/10);
                left_it = left.begin();
                right_it = right.begin();
            }
            template <typename T>
            void wav_sample(const std::vector<T> &sample)
            {
                SS_(log::hr(sample));
                ++sample_count;
                if (sample.size() == 2)
                {
                    const auto a = sample[0];
                    const auto b = sample[1];
#if 0
                    const auto l = a;
                    const auto r = b;
#else
                    const auto l = a+b;
                    const auto r = a-b;
#endif
                    if (left_it != left.end())
                        *left_it++ = l;
                    if (right_it != right.end())
                        *right_it++ = r;
                }
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
    Plot plot;
    plot.scatter(parser.left, parser.right);
    plot.wait();
}
#include "gubg/log/end.hpp"
