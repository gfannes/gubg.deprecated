#ifndef asdf
#define asdf

#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include <vector>
#include <sstream>

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            //Local: #include "a/b/c.hpp"
            //System: #include <a/b/c.hpp>
            enum class IncludeType {Local, System};

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
                            enum State {Idle, IncludeDetected, BeginBracketInclude};
                            State state = Idle;
                            std::ostringstream oss;
                            for (auto &token: lexer.tokens())
                            {
                                typedef pp::Token Token;
                                if (token.isWhitespace())
                                    continue;
                                switch (state)
                                {
                                    case Idle:
                                        if (token.type == Token::Macro && token.range.content() == "#include")
                                            state = IncludeDetected;
                                        break;
                                    case IncludeDetected:
                                        if (token.type == Token::String)
                                        {
                                            const auto content = token.range.content();
                                            MSS(content.front() == '"' && content.back() == '"', IllegalInclude);
                                            MSS(content.size() > 2, IllegalIncludeSize);
                                            receiver_().includes_detected(content.substr(1, content.size()-2), IncludeType::Local);
                                            state = Idle;
                                        }
                                        else if (token.type == Token::Symbol)
                                        {
                                            MSS(token.range.content() == "<");
                                            oss.str("");
                                            state = BeginBracketInclude;
                                        }
                                        else
                                        {
                                            receiver_().includes_error();
                                            MSS_L(UnexpectedToken);
                                        }
                                        break;
                                    case BeginBracketInclude:
                                        switch (token.type)
                                        {
                                            case Token::Identifier:
                                                oss << token.range.content();
                                                break;
                                            case Token::Symbol:
                                                {
                                                    const auto symbol = token.range.content();
                                                    assert(symbol.size() == 1);
                                                    switch (symbol[0])
                                                    {
                                                        case '.':
                                                        case '/':
                                                            oss << symbol;
                                                            break;
                                                        case '>':
                                                            receiver_().includes_detected(oss.str(), IncludeType::System);
                                                            oss.str("");
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
                            MSS_END();
                        }

                        //Default implementation
                        void includes_error(){}

                    private:
                        Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                };
        }
    }
}

#endif
