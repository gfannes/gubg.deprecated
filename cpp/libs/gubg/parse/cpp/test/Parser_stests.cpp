#include "gubg/Testing.hpp"
#include "gubg/parse/cpp/Parser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/l.hpp"
#include <regex>
#include <list>
using namespace gubg::parse::cpp;
using namespace std;
using gubg::file::File;

namespace 
{
    struct ParseTree: gubg::file::Recursor_crtp<ParseTree>
    {
        typedef list<File> Files;
        Files oks;
        Files kos;
        Parser parser;
        typedef gubg::file::ReturnCode ReturnCode;
        ReturnCode recursor_discoveredFile(const File &file)
        {
            MSS_BEGIN(ReturnCode);
            if (file.type() == File::Regular && regex_match(file.name().c_str(), regex(".*\\.cpp")))
            {
                if (gubg::mss::isOK(parser.parse(file)))
                    oks.push_back(file);
                else
                {
                    kos.push_back(file);
                    MSS_L(Stop);
                }
            }
            MSS_END();
        }
    };
}

int main()
{
    ParseTree pt;
    while (true)
    {
        pt.oks.clear();
        pt.kos.clear();
        pt(File("/home/gfannes/gubg/cpp"));
        L("Nr OK: " << pt.oks.size());
        L("Nr KO: " << pt.kos.size());
    }
    return 0;
}