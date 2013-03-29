#ifndef HEADER_gubg_parse_cpp_Includes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_Includes_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include <vector>
#include <sstream>

#define GUBG_MODULE "Includes"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            //Local: #include "a/b/c.hpp"
            //System: #include <a/b/c.hpp>
            enum class IncludeType {Local, System, Macro};

            //Produces following events:
            //includes_detected(const std::string &, IncludeType)
            //includes_error(const std::string &msg) if needed
            template <typename Receiver>
                class Includes_crtp
                {
                    public:
                        ReturnCode process(const gubg::file::File &file)
                        {
                            MSS_BEGIN(ReturnCode);
                            Range range;
                            MSS(gubg::file::read(range, file));
                            pp::Lexer<std::vector<pp::Token>> lexer;
                            MSS(lexer.tokenize(range));
                            enum State {Idle, IncludeDetected, BeginBracketInclude, MacroInclude};
                            State state = Idle;
                            std::ostringstream incl;
                            auto &tokens = lexer.tokens();
                            for (auto it = tokens.begin(); it != tokens.end(); ++it)
                            {
                                auto &token = *it;
                                typedef pp::Token Token;

                                if (state == MacroInclude)
                                {
                                    if (isLineFeed(token) || isCarriageReturn(token))
                                    {
                                        receiver_().includes_detected(incl.str(), IncludeType::Macro);
                                        incl.str("");
                                        state = Idle;
                                    }
                                    else
                                        incl << token.range.content();
                                    continue;
                                }

                                if (token.isWhitespace())
                                    continue;

                                switch (state)
                                {
                                    case Idle:
                                        if (isMacro(token) && token.range.content() == "include")
                                            state = IncludeDetected;
                                        break;
                                    case IncludeDetected:
                                        if (isString(token))
                                        {
                                            const auto content = token.range.content();
                                            MSS(content.front() == '"' && content.back() == '"', IllegalInclude);
                                            MSS(content.size() > 2, IllegalIncludeSize);
                                            receiver_().includes_detected(content.substr(1, content.size()-2), IncludeType::Local);
                                            state = Idle;
                                        }
                                        else if (isSymbol(token))
                                        {
                                            MSS(token.range.content() == "<");
                                            incl.str("");
                                            state = BeginBracketInclude;
                                        }
                                        else if (isIdentifier(token))
                                        {
                                            incl.str("");
                                            incl << token.range.content();
                                            state = MacroInclude;
                                        }
                                        else
                                        {
                                            std::ostringstream oss;
                                            oss << "I expected either a Symbol or a String, but I got a " << toString(token.type);
                                            receiver_().includes_error(oss.str());
                                            MSS_L(UnexpectedToken);
                                        }
                                        break;
                                    case BeginBracketInclude:
                                        switch (token.type)
                                        {
                                            case Token::Identifier:
                                                incl << token.range.content();
                                                break;
                                            case Token::Symbol:
                                                {
                                                    const auto symbol = token.range.content();
                                                    assert(symbol.size() == 1);
                                                    switch (symbol[0])
                                                    {
                                                        case '.':
                                                        case '/':
                                                        case '-':
                                                            incl << symbol;
                                                            break;
                                                        case '>':
                                                            receiver_().includes_detected(incl.str(), IncludeType::System);
                                                            incl.str("");
                                                            state = Idle;
                                                            break;
                                                        default:
                                                            MSS_L(UnexpectedSymbol);
                                                            break;
                                                    }
                                                }
                                                break;
                                            default:
                                                MSS_L(UnexpectedToken);
                                                break;
                                        }
                                        break;
                                }
                            }
                            if (state == MacroInclude)
                            {
                                receiver_().includes_detected(incl.str(), IncludeType::Macro);
                                incl.str("");
                                state = Idle;
                            }
                            MSS_END();
                        }

                        //Default implementation
                        void includes_error(const std::string &){}

                    private:
                        Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                };
        }
    }
}
#include "gubg/log/end.hpp"

#endif
