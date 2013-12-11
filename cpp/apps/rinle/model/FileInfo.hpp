#ifndef HEADER_rinle_model_File_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_File_hpp_ALREADY_INCLUDED

#include "rinle/Codes.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/cpp/pp/Lexer.hpp"
#include "gubg/parse/cpp/Keywords.hpp"
#include "gubg/pattern/Observer.hpp"
#include <string>
#include <cassert>

#define GUBG_MODULE_ "Window"
#include "gubg/log/begin.hpp"
namespace rinle
{
    namespace model
    {
		struct Location
		{
			Lines &lines;
			size_t row;
			size_t col;
			Location(Lines &l): lines(l), row(0), col(0) {}
			Location begin() const
			{
				return Location(lines);
			}
			Location end() const
			{
				Location ret(lines);
				ret.row = lines.size();
				return ret;
			}
			bool operator==(const Location &rhs) const
			{
				assert(&lines == &rhs.lines);
				return std::make_pair(row, col) == std::make_pair(rhs.row, rhs.col);
			}
			bool operator!=(const Location &rhs) const
			{
				assert(&lines == &rhs.lines);
				return std::make_pair(row, col) != std::make_pair(rhs.row, rhs.col);
			}
			bool operator<(const Location &rhs) const
			{
				assert(&lines == &rhs.lines);
				return std::make_pair(row, col) < std::make_pair(rhs.row, rhs.col);
			}
			bool operator<=(const Location &rhs) const
			{
				assert(&lines == &rhs.lines);
				return std::make_pair(row, col) <= std::make_pair(rhs.row, rhs.col);
			}
			Token *operator->() { return &lines[row][col]; }
			Location &operator++()
			{
				while (row < lines.size())
				{
					if (col+1 < lines[row].size())
					{
						++col;
						break;
					}
					else
					{
						++row;
						col = 0;
                        if (row < lines.size() && col < lines[row].size())
                            break;
					}
				}
				return *this;
			}
			Location &operator--()
			{
				if (row == 0 && col == 0)
					return *this;

				//Decrease row until we are within lines
				while (row >= lines.size())
				{
					if (row == 0)
						return *this;
					--row;
					if (row < lines.size())
						col = lines[row].size();
				}
				while (row < lines.size())
				{
					if (col > 0)
					{
						//We found the previous position
						--col;
						return *this;
					}
					if (row == 0)
						return *this;
					--row;
					col = lines[row].size();
				}
				return *this;
			}
			Location &operator+=(int n)
			{
				if (n == 0) {}
				else if (n > 0)
				{
					for (int i = 0; i < n; ++i)
						operator++();
				}
				else if (n < 0)
				{
					for (int i = 0; i < -n; ++i)
						operator--();
				}
				return *this;
			}
		};
        inline std::ostream &operator<<(std::ostream &os, const Location &l)
        {
            return os << "Location(" << l.row << ", " << l.col << ")";
        }

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
	}
}
#include "gubg/log/end.hpp"

#endif