#include "da/FixIncludeGuards.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/l.hpp"
#include "gubg/parse/cpp/Lexer.hpp"
using namespace da;
using namespace gubg::file;
using namespace gubg::parse::cpp;

namespace 
{
    typedef da::ReturnCode ReturnCode;
    ReturnCode fixIncludeGuards(const File &header)
    {
        MSS_BEGIN(ReturnCode);
        MSS_END();
    }

    struct Recursor: gubg::file::Recursor_crtp<Recursor>
    {

        template <typename File>
            ReturnCode recursor_discoveredFile(const File &file)
            {
                MSS_BEGIN(ReturnCode);
                const auto ext = file.extension();
                if (ext == "hpp" || ext == "h")
                {
                    L("This is a header: " << STREAM(file.name()));
                    MSS(fixIncludeGuards(file));
                }
                MSS_END();
            }
    };
}

da::ReturnCode FixIncludeGuards::execute()
{
    MSS_BEGIN(ReturnCode);
    Recursor()(gubg::file::getcwd());
    MSS_END();
}
