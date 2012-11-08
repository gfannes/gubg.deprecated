#ifndef gubg_parse_cpp_Lexer_hpp
#define gubg_parse_cpp_Lexer_hpp

//Converts a Range into Tokens

#include "gubg/parse/cpp/Types.hpp"
#include "gubg/parse/cpp/Token.hpp"
#include "gubg/parse/BreakUp.hpp"

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            class LexerState
            {
                public:
                    typedef Range::iterator It;
                    enum State {Error, Idle, Detecting, Detected};

                    LexerState();

                    bool findToken(It s, It m, It e);

                    ReturnCode getToken(Token &);
                    void setRange(Range range){range_ = range;}

                    bool error(const std::string &msg);

                private:
                    typedef Token::Type Type;
                    bool detectingA_(Type t);
                    bool detectedA_(Type t);
                    bool detected_();
                    void printToken_(char);
                    void printToken_(char, char);

                    State state_;
                    Type type_;
                    bool escaped_;
                    //A counter indicating how many backslash-newline characters have to be skipped
                    int bsnl_;
                    char pch_;
                    Range range_;
                    std::string errorMsg_;
            };

            template <typename Tokens>
                class Lexer
                {
                    public:
                        typedef Range::iterator It;

                        ReturnCode tokenize(Range range)
                        {
                            MSS_BEGIN(ReturnCode);
                            MSS(!range.empty(), RangeIsEmpty);
                            tokens_.clear();
                            lexerState_.setRange(range);
                            MSS(breakUp(range.begin(), range.end(), *this), LexingFailed);
                            MSS_END();
                        }

                        Tokens &tokens(){return tokens_;}

                        bool operator()(It s, It m, It e)
                        {
                            const auto ret = lexerState_.findToken(s, m, e);
                            if (ret)
                            {
                                Token token;
                                if (!mss::isOK(lexerState_.getToken(token)))
                                    lexerState_.error("Could not get token");
                                else
                                    tokens_.push_back(token);
                            }
                            return ret;
                        }
                    private:
                        Tokens tokens_;
                        LexerState lexerState_;
                };
        }
    }
}

#endif
