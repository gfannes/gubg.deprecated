#ifndef HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED

#include "rinle/Codes.hpp"
#include "rinle/model/Types.hpp"
#include "rinle/model/LineNavigator.hpp"
#include "gubg/SmartRange.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/parse/cpp/Keywords.hpp"
#include "gubg/pattern/SignalSlot.hpp"
#include <string>
#include <memory>
#include <cassert>

#define GUBG_MODULE_ "Window"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model {

    class FileInfo: public PageSrc_itf
    {
        public:
            typedef std::shared_ptr<FileInfo> Ptr;

            static Ptr create(const File &path) {return Ptr(new FileInfo(path));}

            void setRubber(bool on)
            {
                if (on)
                    rubber_.reset(new Range(locus_));
                else
                    rubber_.reset();
                refresh();
            }

            void refresh() {signal.emit(*this);}
			typedef const PageSrc_itf &Msg;
            gubg::pattern::Signal<Msg> signal;
            virtual void getPageData(PageData &pd) const
            {
                typedef PageData::AString AString;
                S();L(path());
                pd.title = path().name();

                //Lookup the ix where locus_ starts
                size_t ix;
                if (!lineNavigator_.getLineIX(ix, locus_.begin))
                {
                    //This locus_ is not known, we assume the file is empty
                    for (auto &dstLine: pd.lines)
                    {
                        AString astr("... empty file ...");
                        dstLine.second.push_back(astr);
                    }
                    return;
                }

                //Get the first line of the locus_, we will use it as a start point to fill the PageData lines
                auto srcLine = lineNavigator_.set(Range(locus_.begin, locus_.begin));
                bool foundBegin = false, foundEnd = false;
                for (auto &dstLine: pd.lines)
                {
                    if (!srcLine.empty())
                    {
                        //The border: set the line number, note that we pre-increment to convert ix into a line number
                        {
                            std::ostringstream oss; oss << ++ix;
                            dstLine.first = oss.str();
                        }
                        //The body
                        {
                            std::ostringstream oss;
                            auto tmpLine = srcLine;
                            while (!tmpLine.empty())
                            {
                                auto &token = tmpLine.front();
                                if (&token == &*locus_.begin)
                                    foundBegin = true;
                                if (&token == &*locus_.end)
                                    foundEnd = true;
                                AString astr(token.toString());
                                if (foundBegin && !foundEnd)
                                    astr.flags.set(PageData::Locus);
                                else if (token.type == Token::Identifier)
                                    astr.flags.set(gubg::parse::cpp::isKeyword(token.range) ? PageData::Keyword : PageData::Identifier);
                                dstLine.second.push_back(astr);
                                tmpLine.popFront();
                            }
                        }
                        if (!lineNavigator_.forward(srcLine))
                            srcLine = Range();
                    }
                }
            }

            File path() const {return path_;}

            void proceed(int nrSteps)
            {
                locus_ = lineNavigator_.set(Range(locus_.begin, locus_.begin));
                while (nrSteps > 0)
                {
                    lineNavigator_.forward(locus_);
                    --nrSteps;
                }
                while (nrSteps < 0)
                {
                    lineNavigator_.backward(locus_);
                    ++nrSteps;
                }
                signal.emit(*this);
            }

        private:
            FileInfo(const File &path)
                : path_(path)
                  ,lineNavigator_(tokens_)
        {
            load_();
        }

            ReturnCode load_()
            {
                MSS_BEGIN(ReturnCode);

                std::string content;
                MSS(gubg::file::read(content, path_));

                typedef gubg::SmartRange<std::string> Range;
                Range range(std::move(content));
                gubg::parse::cpp::pp::Lexer<Tokens> lexer;
                MSS(lexer.tokenize(range));

                tokens_ = lexer.tokens();
                lineNavigator_.refresh();

                locus_.begin = locus_.end = tokens_.begin();
                if (locus_.end != tokens_.end())
                    ++locus_.end;

                MSS_END();
            }

            gubg::file::File path_;
            Tokens tokens_;
            Range locus_;
            std::shared_ptr<Range> rubber_;
            LineNavigator lineNavigator_;
    };

} }
#include "gubg/log/end.hpp"

#endif
