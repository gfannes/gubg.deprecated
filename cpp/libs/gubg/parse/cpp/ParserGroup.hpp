#ifndef HEADER_gubg_parse_cpp_ParserGroup_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_ParserGroup_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include <cassert>

#define GUBG_MODULE "ParserGroup"
#include "gubg/log/begin.hpp"
namespace gubg
{
	namespace parse
	{
		namespace cpp
		{
			template <typename Receiver>
				class Identifier_crtp
				{
					public:
						bool processBegin() { return true; }
						bool process(const pp::Token &token)
						{
							if (isIdentifier(token))
								receiver_().identifier_detected(token.range.content());
							return true;
						}
						bool processEnd() { return true; }

					private:
						Receiver &receiver_(){return static_cast<Receiver&>(*this);}
				};

			//Local: #include "a/b/c.hpp"
			//System: #include <a/b/c.hpp>
			enum class IncludeType {Local, System, Macro};

			template <typename Receiver>
				class Include_crtp
				{
					public:
						enum State {Idle, IncludeDetected, BeginBracketInclude, MacroInclude};
						typedef pp::Token Token;

						Include_crtp():
							state_(Idle){}

						bool processBegin()
						{
							receiver_().include_begin();
							return true;
						}
						ReturnCode process(const pp::Token &token)
						{
							MSS_BEGIN(ReturnCode);

							if (state_ == MacroInclude)
							{
								if (isLineFeed(token) || isCarriageReturn(token))
								{
									receiver_().include_detected(include_.str(), IncludeType::Macro);
									include_.str("");
									state_ = Idle;
								}
								else
									include_ << token.range.content();
							}
							else if (token.isWhitespace())
							{
							}
							else
							{
								switch (state_)
								{
									case Idle:
										if (isMacro(token) && token.range.content() == "include")
											state_ = IncludeDetected;
										break;
									case IncludeDetected:
										if (isString(token))
										{
											const auto content = token.range.content();
											MSS(content.front() == '"' && content.back() == '"', IllegalInclude);
											MSS(content.size() > 2, IllegalIncludeSize);
											receiver_().include_detected(content.substr(1, content.size()-2), IncludeType::Local);
											state_ = Idle;
										}
										else if (isSymbol(token))
										{
											MSS(token.range.content() == "<");
											include_.str("");
											state_ = BeginBracketInclude;
										}
										else if (isIdentifier(token))
										{
											include_.str("");
											include_ << token.range.content();
											state_ = MacroInclude;
										}
										else
										{
											std::ostringstream oss;
											oss << "I expected either a Symbol or a String, but I got a " << toString(token.type);
											receiver_().include_error(oss.str());
											MSS_L(UnexpectedToken);
										}
										break;
									case BeginBracketInclude:
										switch (token.type)
										{
											case Token::Identifier:
												include_ << token.range.content();
												break;
											case Token::Symbol:
												{
													const auto symbol = token.range.content();
													assert(symbol.size() == 1);
													switch (symbol[0])
													{
														case '.':
														case '/':
															include_ << symbol;
															break;
														case '>':
															receiver_().include_detected(include_.str(), IncludeType::System);
															include_.str("");
															state_ = Idle;
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
						bool processEnd()
						{
							if (state_ == MacroInclude)
							{
								receiver_().include_detected(include_.str(), IncludeType::Macro);
								include_.str("");
								state_ = Idle;
							}
							receiver_().include_end();
							return true;
						}

						void include_begin(){}
						void include_end(){}
						void include_error(const std::string &str){}

					private:
						Receiver &receiver_(){return static_cast<Receiver&>(*this);}

						State state_;
						std::ostringstream include_;
				};

			template <typename Receiver, template <typename Receiver> class ... Parsers>
				class ParserGroup_crtp: public Parsers<Receiver>...
			{
				public:
					template <template <typename R> class Parser>
					ReturnCode process(const gubg::file::File &file)
					{
						MSS_BEGIN(ReturnCode);

						file_ = file;

						Range range;
						MSS(gubg::file::read(range, file_));
						pp::Lexer<std::vector<pp::Token>> lexer;
						MSS(lexer.tokenize(range));

						MSS(Parser<Receiver>::processBegin());
						for (const auto &token: lexer.tokens())
						{
							MSS(Parser<Receiver>::process(token));
						}
						MSS(Parser<Receiver>::processEnd());

						MSS_END();
					}

					file::File file() const {return file_;}

				private:
					file::File file_;
			};
		}
	}
}
#include "gubg/log/end.hpp"

#endif
