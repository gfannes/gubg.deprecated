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
            void wav_format(const wav::Format &fmt)
            {
                SS(fmt);
                fmt_ = fmt;
            }
            template <typename T>
            void wav_sample(const std::vector<T> &sample)
            {
                SS_(log::hr(sample));
                ++sample_count;
            }
            void wav_done()
            {
                SS(sample_count);
            }

        private:
            wav::Format fmt_;
            size_t sample_count = 0;
    };
} 

TEST_CASE("WAV parser test", "[wav]")
{
    Parser parser;
    REQUIRE(MSS_IS_OK(parser(file::File("piano2.wav"))));
}
#include "gubg/log/end.hpp"
