#ifndef HEADER_gubg_parse_cpp_Functions_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_Functions_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include <vector>
#include <deque>
#include <sstream>

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            //Local: #include "a/b/c.hpp"
            //System: #include <a/b/c.hpp>
            enum class FunctionType {Declaration, Definition, Call};

	    //Produces following events:
	    //includes_detected(const std::string &name, FunctionType)
	    //includes_error(const std::string &msg) if needed
            template <typename Receiver>
                class Functions_crtp
                {
                    public:
			    struct Info
			    {
				    typedef pp::Token Token;

				    enum State {Macro, TypeName, Args, PostArgs, BeginThrowSpec, ThrowSpec, EndThrowSpec};
				    State state;

				    typedef std::deque<Token> Tokens;
				    Tokens type;
				    Tokens name;
				    Tokens args;
				    size_t argsParen;
				    Tokens postArgs;
				    size_t postArgsParen;
				    Tokens throwSpec;
				    size_t throwParen;

				    Info()
				    {
					    clear();
				    }
				    void clear()
				    {
					    state = TypeName;
					    type.clear();
					    name.clear();
					    args.clear();
					    argsParen = 0;
					    postArgs.clear();
					    postArgsParen = 0;
					    throwSpec.clear();
					    throwSpecParen = 0;
				    }
				    void changeState(State newState)
				    {
					    switch (newState)
					    {
						    case Args: argsParen = 0; break;
						    case Macro: clear(); break;
						    case PostArgs: postArgsParen = 0; break;
						    case BeginThrowSpec: throwSpecParen = 0; break;
					    }
					    state = newState;
				    }
				    void addTypeName(const Token &token)
				    {
					    name.push_back(token);
				    }
				    void addArgs(const Token &token)
				    {
					    args.push_back(token);
				    }
				    void addPostArgs(const Token &token)
				    {
					    postArgs.push_back(token);
				    }

				    bool getName(std::string &res) const
				    {
					    if (name.empty())
						    return false;
					    res = name.back().range.content();
					    return true;
				    }
			    };
			    ReturnCode process(const gubg::file::File &file)
			    {
				    MSS_BEGIN(ReturnCode);

				    typedef pp::Token Token;

				    //Read and tokenize file
				    Range range;
				    MSS(gubg::file::read(range, file));
				    pp::Lexer<std::vector<Token>> lexer;
				    MSS(lexer.tokenize(range));

				    Info info;

				    for (auto &token: lexer.tokens())
				    {
					    if (info.state == Info::Macro)
					    {
						    if (isLineFeed(token) || isCarriageReturn(token))
							    info.clear();
						    continue;
					    }
					    if (token.isWhitespace())
						    continue;
					    if (isMacro(token))
					    {
						    info.changeState(Info::Macro);
						    continue;
					    }

					    switch (info.state)
					    {
						    case Info::TypeName:
							    switch (token.type)
							    {
								    case Token::Identifier:
									    info.addTypeName(token);
									    break;
								    case Token::Symbol:
									    if (isSymbol(token, '('))
										    info.changeState(Info::Args);
									    else if (isSymbolFrom(token, ":<>"))
										    info.addTypeName(token);
									    else
										    info.clear();
									    break;
							    }
							    break;
						    case Info::Args:
							    switch (token.type)
							    {
								    case Token::Identifier:
									    info.addArgs(token);
									    break;
								    case Token::Symbol:
									    if (isSymbol(token, '('))
										    ++info.argsParen;
									    else if (isSymbol(token, ')'))
									    {
										    if (info.argsParen == 0)
										    {
											    info.changeState(Info::PostArgs);
											    break;
										    }
										    --info.argsParen;
									    }
									    else if (isSymbol(token, ';'))
									    {
										    info.clear();
										    break;
									    }
									    info.addArgs(token);
									    break;
							    }
							    break;
						    case Info::PostArgs:
							    switch (token.type)
							    {
								    case Token::Identifier:
									    {
										    const auto ident = token.range.content();
										    if (ident == "const" || ident == "volatile")
										    {
											    info.addPostArgs(token);
										    }
										    else if (ident == "throw" || ident == "raises")
										    {
											    info.addPostArgs(token);
											    info.changeState(BeginThrowSpec);
										    }
										    else
										    {
											    info.clear();
											    break;
										    }
									    }
									    break;
								    case Token::Symbol:
									    if (isSymbol(token, ';'))
									    {
										    std::string name;
										    if (info.getName(name))
											    receiver_().functions_detected(name, FunctionType::Declaration);
										    info.clear();
									    }
									    else if (isSymbol(token, '{'))
									    {
										    std::string name;
										    if (info.getName(name))
											    receiver_().functions_detected(name, FunctionType::Declaration);
									    }
									    else if (isSymbol(token, '}'))
									    {
										    if (info.postArgsParen == 0)
										    {
											    info.clear();
											    break;
										    }
										    --info.postArgsParen;
									    }
									    break;
							    }
							    break;
						    case Info::BeginThrowSpec:
							    if (!isSymbol(token, '('))
								    info.clear();
							    else
								    info.changeState(Info::ThrowSpec);
							    break;
						    case Info::ThrowSpec:
							    switch (token.type)
							    {
								    case Token::Identifier:
									    info.addThrowSpec(token);
									    break;
								    case Token::Symbol:
									    if (isSymbol(token, '('))
										    ++info.throwSpecParen;
									    else if (isSymbol(token, ')'))
									    {
										    if (info.throwSpecParen == 0)
										    {
											    info.changeState(Info::EndThrowSpec);
											    continue;
										    }
										    --info.throwSpecParen;
									    }
									    break;
							    }
							    break;
						    case Info::EndThrowSpec:
							    if (!isSymbol(token, '('))
								    info.clear();
							    else
								    info.changeState(Info::ThrowSpec);
							    break;
					    }
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

#endif
