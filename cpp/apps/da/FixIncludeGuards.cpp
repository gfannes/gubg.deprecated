#include "da/FixIncludeGuards.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/string_algo.hpp"
#include <vector>
#include <cassert>
using namespace da;
using namespace gubg::file;
using namespace gubg::parse::cpp;
using namespace gubg::parse::cpp::pp;
using namespace std;

#define GUBG_MODULE "FixIncludeGuards"
#include "gubg/log/begin.hpp"
namespace 
{
    typedef da::ReturnCode ReturnCode;
    typedef vector<Token> Tokens;

    class Recursor
    {
        public:
            Recursor(const Options &options):
                options_(options){}

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
            ReturnCode fix_(const string &headerLocation)
            {
                MSS_BEGIN(ReturnCode);

                Token *ident1(0), *ident2(0), *lastMacro(0);
                enum State {Start, IfndefDetected, Ident1Detected, DefineDetected, Ident2Detected, LastMacroDetected, Error};
                State state = Start;
                for (auto &token: lexer_.tokens())
                {
                    if (token.isWhitespace())
                    {
                        MSS_Q(state != Start, NoIncludeGuardFound);
                        continue;
                    }

                    switch (state)
                    {
                        case Start:
                            if (token.type == Token::MacroHash)
                                continue;
                            MSS(token.type == Token::Macro);
                            MSS(token.range.content() == "ifndef", NoIncludeGuardFound);
                            state = IfndefDetected;
                            break;
                        case IfndefDetected:
                            MSS(token.type == Token::Identifier);
                            ident1 = &token;
                            state = Ident1Detected;
                            break;
                        case Ident1Detected:
                            if (token.type == Token::MacroHash)
                                continue;
                            MSS(token.type == Token::Macro);
                            MSS(token.range.content() == "define");
                            state = DefineDetected;
                            break;
                        case DefineDetected:
                            MSS(token.type == Token::Identifier);
                            ident2 = &token;
                            state = Ident2Detected;
                            break;
                        case Ident2Detected:
                        case LastMacroDetected:
                            if (token.type == Token::Macro)
                            {
                                lastMacro = &token;
                                state = LastMacroDetected;
                            }
                            break;
                    }
                }

                MSS(state == LastMacroDetected);
                assert(ident1 != 0 && ident2 != 0 && lastMacro != 0);
                MSS(lastMacro->range.content() == "endif");

                ident1->range = headerLocation;
                ident2->range = headerLocation;

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
                        case ':':
                        case '\\':
                            ch = '_';
                            break;
                    }
                return string("HEADER_") + wd_.basename() + "_" + ig + "_ALREADY_INCLUDED";
            }
            ReturnCode fixIncludeGuards_(const File &header)
            {
                MSS_BEGIN(ReturnCode, STREAM(header.name()));
                MSS(tokenize_(header));
                switch (auto rc = fix_(includeGuard_(header)))
                {
                    case ReturnCode::NoIncludeGuardFound:
                        //Some headers do not use include guards
                        break;
                    default:
                        MSS(rc);
                        if (content_() != range_.content())
                        {
                            L("Header " << header.name() << " needs fixing");
                            //                    L("Orig content:\n" << range_.content());
                            //                    L("New content:\n" << content_());
                            if (options_.doFix)
                                MSS(write(content_(), header));
                        }
                        break;
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
    Recursor recursor(options);
    MSS(gubg::file::recurse(recursor));
    MSS_END();
}
