#include "catch/catch.hpp"
#include "gubg/toml/Decoder.hpp"
using namespace gubg;
using namespace std;

namespace data { 
	const string number = "number=42"; 
} 

namespace  { 
	struct Decoder: toml::Decoder_crtp<Decoder>
	{
	};
} 
TEST_CASE("Decoder", "[toml]")
{
	using toml::ReturnCode;
	Decoder decoder;
	REQUIRE(ReturnCode::OK == decoder.decode(data::number));
}
