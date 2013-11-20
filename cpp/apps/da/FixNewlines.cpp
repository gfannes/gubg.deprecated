#include "da/FixNewlines.hpp"
#include "gubg/file/Filesystem.hpp"
#include <vector>
#include <cassert>
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
                            //if (options_.doFix)
                              //  MSS(write(content_(), header));
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
