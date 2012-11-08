#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "da/FixIncludeGuards.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/l.hpp"
#include "gubg/parse/cpp/Lexer.hpp"
#include "gubg/string_algo.hpp"
#include <vector>
using namespace da;
using namespace gubg::file;
using namespace gubg::parse::cpp;
using namespace std;

namespace 
{
    typedef da::ReturnCode ReturnCode;
    typedef vector<Token> Tokens;

    class Recursor: public gubg::file::Recursor_crtp<Recursor>
    {
        private:
            typedef gubg::file::Recursor_crtp<Recursor> Base;

        public:
            Recursor(const Options &options, const File &wd):
                options_(options), wd_(wd){}

            ReturnCode operator()()
            {
                MSS_BEGIN(ReturnCode);
                MSS(Base::operator()(wd_));
                MSS_END();
            }

            template <typename File>
                ReturnCode recursor_discoveredFile(const File &file)
                {
                    MSS_BEGIN(ReturnCode);
                    const auto ext = file.extension();
                    if (ext == "hpp" || ext == "h")
                        MSS(fixIncludeGuards_(file));
                    MSS_END();
                }

        private:
            ReturnCode tokenize_(const File &header)
            {
                MSS_BEGIN(ReturnCode);
                MSS(read(range_, header));
                MSS(lexer_.tokenize(range_));
                MSS_END();
            }
            string content_()
            {
                return gubg::string_algo::join(lexer_.tokens(), "");
            }
            static ReturnCode startsWith_(const Token &token, const string &start)
            {
                MSS_BEGIN(ReturnCode);
                MSS(token.range.content().substr(0, start.size()) == start);
                MSS_END();
            }
            ReturnCode fix_(const string &headerLocation)
            {
                MSS_BEGIN(ReturnCode);
                Token *m1(0), *m2(0), *mlast(0);
                for (auto &token: lexer_.tokens())
                {
                    switch (token.type)
                    {
                        case Token::LineComment:
                        case Token::BlockComment:
                        case Token::Blanks:
                        case Token::Newline:
                            break;
                        case Token::Macro:
                            if (!m1)
                                m1 = &token;
                            else if (!m2)
                                m2 = &token;
                            else
                                //We keep track of the last macro we see
                                mlast = &token;
                            break;
                        default:
                            //We expect the first non blank or newline or macro token _after_ the ifndef/define of the include guard
                            MSS(m1 != 0 && m2 != 0);
                            break;
                    }
                }
                MSS(m1 != 0 && m2 != 0 && mlast != 0);
                MSS(startsWith_(*m1, "#ifndef "));
                MSS(startsWith_(*m2, "#define "));
                MSS(startsWith_(*mlast, "#endif"));

                {
                    ostringstream oss;
                    oss << "#ifndef " << headerLocation;
                    m1->range = oss.str();
                    oss.str("");
                    oss << "#define " << headerLocation;
                    m2->range = oss.str();
                }
                MSS_END();
            }
            string includeGuard_(const File &header)
            {
                auto ig = header.relative(wd_).name();
                for (auto &ch: ig)
                    switch (ch)
                    {
                        case '.':
                        case '/':
                            ch = '_';
                            break;
                    }
                return string("HEADER_") + wd_.basename() + "_" + ig + "_ALREADY_INCLUDED";
            }
            ReturnCode fixIncludeGuards_(const File &header)
            {
                MSS_BEGIN(ReturnCode, fixIncludeGuards_, header.name());
                MSS(tokenize_(header));
                MSS(fix_(includeGuard_(header)));
                if (content_() != range_.content())
                {
                    L("Header " << header.name() << " needs fixing");
                    if (options_.doFix)
                        MSS(write(content_(), header));
                }
                MSS_END();
            }
            const Options &options_;
            const File wd_;
            Range range_;
            Lexer<Tokens> lexer_;
    };
}

da::ReturnCode FixIncludeGuards::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode);
    Recursor recursor(options, gubg::file::getcwd());
    recursor();
    MSS_END();
}
