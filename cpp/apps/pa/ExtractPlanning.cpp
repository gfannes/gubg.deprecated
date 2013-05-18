#include "pa/ExtractPlanning.hpp"
#include "gubg/parse/xml/Parser.hpp"
using namespace pa;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "ExtractPlanning"
#include "gubg/log/begin.hpp"
ExtractPlanning::ExtractPlanning(const string &file):
    file_(file)
{
}

namespace pa
{
    struct Node
    {
        typedef std::string Name;
        typedef double Value;
        typedef std::vector<Node> Childs;

        Name name;
        Value value;
        Childs childs;
    };
}
pa::ReturnCode ExtractPlanning::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode, STREAM(file_));

    MSS_END();
}
