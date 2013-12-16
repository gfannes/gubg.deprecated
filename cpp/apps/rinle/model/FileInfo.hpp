#ifndef HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED

#include "rinle/Codes.hpp"
#include "rinle/model/Location.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/parse/cpp/Keywords.hpp"
#include "gubg/pattern/Observer.hpp"
#include <string>
#include <cassert>

#define GUBG_MODULE_ "Window"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model {

    class FileInfo
    {
        public:
            typedef std::shared_ptr<FileInfo> Ptr;
            typedef std::vector<gubg::parse::cpp::pp::Token> PPTokens;

            static Ptr create(const File &path) {return Ptr(new FileInfo(path));}

            File path() const {return path_;}
            size_t selectionStart() const {return begin_.row;}

            const Lines &lines() const
            {
                return lines_;
            }

            bool getLines(Lines &lines, size_t startLine, size_t nrLines) const
            {
                lines.clear();
                for (size_t i = 0; i < nrLines; ++i)
                {
                    const auto ix = startLine + i;
                    if (ix >= lines_.size())
                        break;
                    lines.push_back(lines_[ix]);
                }
                return true;
            }

            void proceed(int nrSteps)
            {
                begin_ += nrSteps;
                end_ += nrSteps;
                if (begin_ == end_)
                    ++end_;
                computeSelection_();
            }

        private:
            FileInfo(const File &path):
                path_(path),
                begin_(lines_), end_(lines_)
        {
            load_();
        }

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

                ++end_;
                computeSelection_();

                MSS_END();
            }
            void computeSelection_()
            {
                for (auto it = Location(lines_); it != it.end(); ++it)
                    it->isSelected = (begin_ <= it && it < end_);
            }

            gubg::file::File path_;
            Lines lines_;
            Location begin_;
            Location end_;
    };

} }
#include "gubg/log/end.hpp"

#endif
