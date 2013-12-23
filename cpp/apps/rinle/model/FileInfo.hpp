#ifndef HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED

#include "rinle/Codes.hpp"
#include "rinle/model/Types.hpp"
#include "rinle/model/LineNavigator.hpp"
#include "rinle/model/TokenNavigator.hpp"
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

                //Lookup the line and ix where locus_ starts
                size_t ix;
				Range line = lineNavigator_.getLine(&ix, locus_.begin());
                if (line.empty())
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
                bool foundBegin = false, foundEnd = false;
                for (auto &dstLine: pd.lines)
                {
                    if (!line.empty())
                    {
                        //The border: set the line number, note that we pre-increment to convert ix into a line number
                        {
                            std::ostringstream oss; oss << ++ix;
                            dstLine.first = oss.str();
                        }
                        //The body
                        {
                            std::ostringstream oss;
                            auto tmpLine = line;
                            while (!tmpLine.empty())
                            {
                                auto &ot = tmpLine.front();
                                if (&ot == &*locus_.begin())
                                    foundBegin = true;
                                if (&ot == &*locus_.end())
                                    foundEnd = true;
                                AString astr(ot.token.toString());
                                if (foundBegin && !foundEnd)
                                    astr.flags.set(PageData::Locus);
                                else if (ot.token.type == Token::Identifier)
                                    astr.flags.set(gubg::parse::cpp::isKeyword(ot.token.range) ? PageData::Keyword : PageData::Identifier);
                                dstLine.second.push_back(astr);
                                tmpLine.popFront();
                            }
                        }
                        if (!lineNavigator_.forward(line))
                            line.clear();
                    }
                }
            }

            File path() const {return path_;}

            void move(Direction dir)
            {
				if (!navigator_)
					return;
				if (navigator_->move(locus_, dir))
					signal.emit(*this);
            }
			void setMode(NavigatorMode mode)
			{
				Navigator *nav = nullptr;
				switch (mode)
				{
					case ByLine: nav = &lineNavigator_; break;
					case ByToken: nav = &tokenNavigator_; break;
				}
				if (nav != navigator_)
				{
					navigator_ = nav;
					locus_ = navigator_->set(Range(locus_.begin(), locus_.begin()));
				}
			}

        private:
            FileInfo(const File &path)
                : path_(path)
                  ,lineNavigator_(tokens_)
                  ,tokenNavigator_(tokens_)
        {
            load_();
			setMode(ByLine);
        }

            ReturnCode load_()
            {
                MSS_BEGIN(ReturnCode);

                std::string content;
                MSS(gubg::file::read(content, path_));

				{
					typedef gubg::SmartRange<std::string> Range;
					typedef std::vector<Token> Tokens;
					Range range(std::move(content));
					gubg::parse::cpp::pp::Lexer<Tokens> lexer;
					MSS(lexer.tokenize(range));
					for (auto it = lexer.tokens().begin(); it != lexer.tokens().end(); ++it)
					{
						OrderedToken ot(*it, it-lexer.tokens().begin());
						tokens_.push_back(ot);
					}
				}
                lineNavigator_.refresh();

				auto locusEnd = tokens_.begin();
				if (locusEnd != tokens_.end())
					++locusEnd;
				locus_ = Range(tokens_.begin(), locusEnd);

                MSS_END();
            }

            gubg::file::File path_;
            Tokens tokens_;
            Range locus_;
            std::shared_ptr<Range> rubber_;
            LineNavigator lineNavigator_;
            TokenNavigator tokenNavigator_;
			Navigator *navigator_ = nullptr;
    };

} }
#include "gubg/log/end.hpp"

#endif
