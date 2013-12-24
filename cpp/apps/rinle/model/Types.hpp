#ifndef HEADER_rinle_model_Types_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Types_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/Range.hpp"
#include <string>
#include <list>
#include <cassert>

namespace rinle { namespace model {

    typedef gubg::parse::cpp::pp::Token Token;
	struct OrderedToken
	{
		Token token;
		long order;
		OrderedToken(): order(-1) {}
		OrderedToken(const Token &tkn, long o): token(tkn), order(o) {}
	};
    typedef std::list<OrderedToken> Tokens;
	typedef gubg::Range<Tokens::iterator> Range;

	enum Direction {Forward, Backward, In, Out};
	enum NavigatorMode {ByToken, ByScope, ByLine};

} }

//We need operator<() to be able to use Range:contains etc.
inline bool operator<(const rinle::model::Tokens::iterator &lhs, const rinle::model::Tokens::iterator &rhs)
{
	return lhs->order < rhs->order;
}

#endif
