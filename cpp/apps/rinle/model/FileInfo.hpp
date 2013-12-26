#ifndef HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_FileInfo_hpp_ALREADY_INCLUDED

#include "rinle/Codes.hpp"
#include "rinle/model/Types.hpp"
#include "rinle/model/Tokens.hpp"
#include "rinle/model/TokenNavigator.hpp"
#include "rinle/model/ScopeNavigator.hpp"
#include "rinle/model/LineNavigator.hpp"
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
                {
                    auto line = lineNavigator_.getLine(&ix, locus_.begin());
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
                }
                //We try to keep the start of the locus_ at 1/4th of the screen
                const auto focusIX = ix;
                ix = (ix < pd.lines.size()/4) ? 0 : ix-pd.lines.size()/4;
                L(ix);

                //Get the first line of the locus_, we will use it as a start point to fill the PageData lines
                bool foundBegin = false, foundEnd = false;
                for (auto &dstLine: pd.lines)
                {
                    auto line = lineNavigator_.getLine(ix);
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
					case ByToken: nav = &tokenNavigator_; break;
					case ByScope: nav = &scopeNavigator_; break;
					case ByLine: nav = &lineNavigator_; break;
				}
				if (!nav)
					return;
				if (nav != navigator_)
				{
					navigator_ = nav;
					navigator_->set(locus_);
					signal.emit(*this);
				}
			}

        private:
            FileInfo(const File &path)
                : path_(path)
                  ,tokenNavigator_(tokens_)
                  ,scopeNavigator_(tokens_)
                  ,lineNavigator_(tokens_)
        {
            load_();
			setMode(ByLine);
        }

            ReturnCode load_()
            {
                MSS_BEGIN(ReturnCode);

                std::string content;
                MSS(gubg::file::read(content, path_));

                MSS(tokenize(tokens_, std::move(content)));

                lineNavigator_.refresh();
				scopeNavigator_.refresh();

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
            TokenNavigator tokenNavigator_;
            ScopeNavigator scopeNavigator_;
            LineNavigator lineNavigator_;
			Navigator *navigator_ = nullptr;
    };

} }
#include "gubg/log/end.hpp"

#endif
