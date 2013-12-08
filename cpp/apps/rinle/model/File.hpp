#ifndef HEADER_rinle_model_File_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_File_hpp_ALREADY_INCLUDED

#include "rinle/Codes.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/parse/cpp/Keywords.hpp"
#include <string>

#define GUBG_MODULE_ "Window"
#include "gubg/log/begin.hpp"
namespace rinle
{
    namespace model
    {
        typedef gubg::SmartRange<std::string> Range;

        struct Token
        {
            bool isSelected = false;
            bool isKeyword = false;
            bool isIdentifier = false;
            Range range;
        };

        class File
        {
            public:
				typedef std::vector<gubg::parse::cpp::pp::Token> PPTokens;
				typedef std::vector<Token> Tokens;
				typedef std::vector<Tokens> Lines;

                File(const std::string &path): path_(path) {load_();}

                Lines lines() const
                {
					return lines_;
                }

            private:
                ReturnCode load_()
                {
					MSS_BEGIN(ReturnCode);

					std::string content;
                    MSS(gubg::file::read(content, path_));

					using namespace gubg::parse::cpp;
					pp::Lexer<PPTokens> lexer;
					Range range(std::move(content));
					MSS(lexer.tokenize(range));

					lines_.push_back(Tokens());
					for (auto token: lexer.tokens())
					{
                        Token t; t.range = token.range;
						switch (token.type)
						{
							case pp::Token::LineFeed:
								lines_.push_back(Tokens());
                                continue;
								break;
							case pp::Token::CarriageReturn:
                                continue;
								break;
							case pp::Token::Identifier:
                                t.isIdentifier = true;
                                break;
						}
                        if (gubg::parse::cpp::isKeyword(t.range))
                            t.isKeyword = true;
                        lines_.back().push_back(t);
					}

					MSS_END();
                }

                gubg::file::File path_;
				Lines lines_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
