#include "gubg/parse/cpp/pp/Token.hpp"
using namespace gubg::parse::cpp::pp;

const char *gubg::parse::cpp::pp::toString(Token::Type type)
{
	switch (type)
	{
#define L_CASE(name) case Token::name: return #name; break
		L_CASE(Unknown);
		L_CASE(MacroHash);
		L_CASE(Macro);
		L_CASE(String);
		L_CASE(Character);
		L_CASE(Identifier);
		L_CASE(Blanks);
		L_CASE(LineFeed);
		L_CASE(CarriageReturn);
		L_CASE(Symbol);
		L_CASE(LineComment);
		L_CASE(BlockComment);
		L_CASE(StandaloneBackslashNewline);
	}
	return "UnknownTokenType";
}
