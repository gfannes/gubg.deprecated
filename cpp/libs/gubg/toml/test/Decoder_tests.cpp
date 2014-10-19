#include "catch/catch.hpp"
#include "gubg/toml/Decoder.hpp"
using namespace gubg;
using namespace std;

namespace data { 
	const string number = "number=42"; 
	const string str = "str=\"eat this\""; 
	const string empty_str = "str=\"\""; 
	const string escaped_str = "str=\"\\\"\""; 
	const string unescaped_str = "str=\'\\\"\'"; 
	const string unescaped_ml_str = "str=\'\'\'\\\"\'\'\'"; 
	const string escaped_ml_str = "str=\"\"\"\\\"\"\"\""; 
	const string path = "[a.b.c]"; 
	const string path_number = "[a.b.c]\nnumber=42"; 
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
	REQUIRE(ReturnCode::OK == decoder.decode(data::str));
	REQUIRE(ReturnCode::OK == decoder.decode(data::empty_str));
	REQUIRE(ReturnCode::OK == decoder.decode(data::escaped_str));
	REQUIRE(ReturnCode::OK == decoder.decode(data::unescaped_str));
	REQUIRE(ReturnCode::OK == decoder.decode(data::unescaped_ml_str));
	REQUIRE(ReturnCode::OK == decoder.decode(data::path));
	REQUIRE(ReturnCode::OK == decoder.decode(data::path_number));
}
