//!@#adoc
//rip: recusive include processor
#include "rip/Codes.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/lua/State.hpp"
#include "lua.hpp"
#include <iostream>
#include <cstdlib>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "rip"
#include "gubg/log/begin.hpp"
namespace lua_ { 
    extern "C"
    {
        int process(lua_State *s)
        {
            SS();LL(s);
            return 0;
        }
    }
} 
namespace rip { 
    struct Options
    {
        gubg::file::File input;
    };
    ostream &operator<<(ostream &os, const Options &options)
    {
        os << "Input file: " << options.input.name();
        return os;
    }

    void finalize(const string &msg, int ret)
    {
        cout << msg << endl;
        exit(ret);
    }

    ReturnCode main(int argc, char **argv)
    {
        MSS_BEGIN(ReturnCode);
        Options options;
        {
            auto args = OptionParser::createArgs(argc, argv);
            OptionParser op("Recursive include processor");
            op.addSwitch("-h", "--help", "Displays this help", [&](){finalize(op.help(), 0);});
            op.addMandatory("-i", "--input", "Input file with lua process({src=\"...\", dep=\"...\"}) calls", [&](const string &fn){options.input = fn;});
            MSS(op.parse(args));
        }
        cout << "options:" << endl << options << endl;
        MSS(!options.input.empty(), InputNotSpecified);
        string input_content;
        MSS(file::read(input_content, options.input), InputFileNotFound);

        auto ls = lua::State::create();
        MSS(ls.registerFunction(lua_::process, "process"));
        MSS(ls.execute(input_content));

        MSS_END();
    } 
}
#include "gubg/log/end.hpp"

int main(int argc, char **argv)
{
    const auto rc = rip::main(argc, argv);
    if (!MSS_IS_OK(rc))
    {
        cout << "ERROR: ";
        switch (rc)
        {
            case rip::ReturnCode::InputNotSpecified: cout << "Input was not specified"; break;
            case rip::ReturnCode::InputFileNotFound: cout << "Could not find the input file"; break;
        }
        cout << endl;
        return -1;
    }

    cout << "Everything went OK" << endl;
    return 0;
}
