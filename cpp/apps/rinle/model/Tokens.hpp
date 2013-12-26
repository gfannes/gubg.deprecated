#ifndef HEADER_rinle_model_Tokens_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Tokens_hpp_ALREADY_INCLUDED

#include "rinle/model/Types.hpp"
#include "rinle/Codes.hpp"
#include "gubg/SmartRange.hpp"
#include <vector>
#include <string>

#define GUBG_MODULE "Tokens"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model {

    inline ReturnCode tokenize(Tokens &tokens, std::string &&content)
    {
        MSS_BEGIN(ReturnCode);

        typedef gubg::SmartRange<std::string> Range;
        typedef std::vector<Token> Tokens;
        Range range(std::move(content));
        gubg::parse::cpp::pp::Lexer<Tokens> lexer;
        MSS(lexer.tokenize(range));
        tokens.clear();
        for (auto it = lexer.tokens().begin(); it != lexer.tokens().end(); ++it)
        {
            OrderedToken ot(*it, it-lexer.tokens().begin());
            tokens.push_back(ot);
        }
        MSS_END();
    }

} }
#include "gubg/log/end.hpp"

#endif
