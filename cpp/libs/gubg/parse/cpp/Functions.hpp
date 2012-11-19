#ifndef HEADER_gubg_parse_cpp_Includes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_Includes_hpp_ALREADY_INCLUDED

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
            enum class FunctionType {Declaration, Definition, Call};

	    //Produces following events:
	    //includes_detected(const std::string &name, FunctionType)
	    //includes_error(const std::string &msg) if needed
            template <typename Receiver>
                class Functions_crtp
                {
                    public:
                        ReturnCode process(const gubg::file::File &file)
                        {
                            MSS_BEGIN(ReturnCode);
                            Range range;
                            MSS(gubg::file::read(range, file));
                            pp::Lexer<std::vector<pp::Token>> lexer;
                            MSS(lexer.tokenize(range));
                            enum State {Normal, Potential, Macro};
                            State state = Normal;
			    std::string name;
			    for (auto &token: lexer.tokens())
                            {
                                typedef pp::Token Token;

				if (isMacro(token))
					state = Macro;

				switch (state)
				{
					case Normal:
						if (isIdentifier(token))
						{
							name = token.range.content();
							state = Potential;
						}
						break;
					case Potential:
						if (isSymbol(token) && token.range.content() == "(")
							receiver_().functions_detected(token.range.content(), FunctionType::Call);
						else if (token.isWhitespace()){}
						else
							state = Normal;
						break;
					case Macro:
						if (isLineFeed(token) || isCarriageReturn(token))
							state = Normal;
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
