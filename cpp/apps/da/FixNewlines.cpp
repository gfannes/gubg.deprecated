#include "da/FixNewlines.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
#include <vector>
#include <cassert>
#include <iostream>
using namespace da;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "FixNewlines"
#include "gubg/log/begin.hpp"
namespace 
{
    typedef da::ReturnCode ReturnCode;

    class Recursor
    {
        public:
            Recursor(const Options &options):
                options_(options),
                wd_(getcwd()){}

            template <typename File>
                ReturnCode recursor_discoveredFile(const File &file)
                {
                    MSS_BEGIN(ReturnCode);
                    const auto ext = file.extension();
                    if (ext == "hpp" || ext == "h" || ext == "cpp" || ext == "c")
                        MSS(fixNewLines_(file));
                    MSS_END();
                }

        private:
            ReturnCode fixNewLines_(const File &file)
            {
                MSS_BEGIN(ReturnCode, STREAM(file.name()));
				string content;
				MSS(read(content, file));
				auto lines = gubg::line::split(content);
				gubg::line::Line::End end;
				bool doFix = false;
				if (analyseEnds(end, lines))
				{
					//One line ending found
					if (end != gubg::line::Line::Unix)
					{
						cout << file << ": " << gubg::line::to_s(end) << " line ending found" << endl;
						doFix = true;
					}
				}
				else
				{
					//Mixed line endings found
					doFix = true;
					cout << file << ": mixed line endings found" << endl;
				}
				if (doFix)
				{
					if (options_.doFix)
					{
						content = gubg::line::join(lines, gubg::line::Line::Unix);
						MSS(write(content, file));
					}
				}

                MSS_END();
            }
            const Options &options_;
            const File wd_;
    };
}

da::ReturnCode FixNewlines::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode);
    Recursor recursor(options);
    MSS(gubg::file::recurse(recursor));
    MSS_END();
}
