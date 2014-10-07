#include "catch/catch.hpp"
#include "gubg/base64/Decode.hpp"
#include <vector>
using namespace std;
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace  { 
	struct Decoder: public base64::Decoder_crtp<Decoder>
	{
		ostringstream oss;
		void base64_decoded(uint8_t dec)
		{
			oss << (char)dec;
		}
	};
} 
TEST_CASE("Decode_crtp", "[base64]")
{
	S();
	Decoder decoder;
	decoder.process('Z');
	decoder.process('g');
	decoder.process('=');
	decoder.process('=');
	L(decoder.oss.str());
}
TEST_CASE("Decode", "[base64]")
{
	S();
	struct TV
	{
		string plain;
		string enc;
	};
	vector<TV> tvs = {
		{"", ""},
		{"f", "Zg=="},
		{"fo", "Zm8="},
		{"foo", "Zm9v"},
		{"foob", "Zm9vYg=="},
		{"fooba", "Zm9vYmE="},
		{"foobar", "Zm9vYmFy"},
	};
	string plain;
	for (auto &tv: tvs)
	{
		L(STREAM(tv.plain, tv.enc));
		REQUIRE(base64::ReturnCode::OK == base64::decode(plain, tv.enc));
		REQUIRE(tv.plain == plain);
	}
}
#include "gubg/log/end.hpp"
