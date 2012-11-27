#ifndef gubg_parse_cpp_ParserGroup_hpp
#define gubg_parse_cpp_ParserGroup_hpp

#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"

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
						bool process(const pp::Token &token)
						{
							if (isIdentifier(token))
								receiver_().identifier_detected(token.range.content());
							return true;
						}
					private:
						Receiver &receiver_(){return static_cast<Receiver&>(*this);}
				};

			template <typename Receiver, template <typename Receiver> class Parser>
				class ParserGroup_crtp: public Parser<Receiver>
			{
				public:
					ReturnCode operator()(const gubg::file::File &file)
					{
						MSS_BEGIN(ReturnCode);

						file_ = file;

						Range range;
						MSS(gubg::file::read(range, file_));
						pp::Lexer<std::vector<pp::Token>> lexer;
						MSS(lexer.tokenize(range));

						for (const auto &token: lexer.tokens())
						{
							MSS(process(token));
						}

						MSS_END();
					}

					file::File file() const {return file_;}

				private:
					file::File file_;
			};
		}
	}
}

#endif
