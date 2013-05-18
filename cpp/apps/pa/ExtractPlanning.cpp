#include "pa/ExtractPlanning.hpp"
#include "gubg/parse/xml/Parser.hpp"
#include "gubg/file/Filesystem.hpp"
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

        Node():value(){}
    };
    struct Parser: gubg::xml::Parser_crtp<Parser>
    {
        Node root;
        typedef std::vector<Node*> Location;
        Location location;

        void parser_open(const string &tag, const Path &path)
        {
            if (tag == "node")
            {
                SS(tag);
                if (location.empty())
                    location.push_back(&root);
                else
                {
                    location.back()->childs.push_back(Node());
                    location.push_back(&location.back()->childs.back());
                }
            }
        }
        void parser_close(const string &tag, const Path &path)
        {
            if (tag == "node")
            {
                SS(tag);
                location.pop_back();
            }
        }
        pa::ReturnCode parser_attr(const Attributes &attrs, const Path &path)
        {
            MSS_BEGIN(pa::ReturnCode);
            if (path.back() == "node")
            {
                auto it = attrs.find("TEXT");
                MSS(it != attrs.end());
                L(it->second);
                location.back()->name = it->second;
            }
            if (path.back() == "attribute")
            {
                auto n = attrs.find("NAME");
                auto v = attrs.find("VALUE");
                MSS(n != attrs.end() && v != attrs.end());
                if (n->second == "effort")
                {
                    MSS(!location.empty());
                    location.back()->value = std::stod(v->second);
                    L("Detected effort for " << location.back()->name << ": " << location.back()->value);
                }
            }
            MSS_END();
        }
    };
}
pa::ReturnCode ExtractPlanning::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode, STREAM(file_));

    string xml;
    MSS(read(xml, file_));

    Parser p;
    MSS(p.process(xml));

//    gubg::tree::dfs::iterate();

    MSS_END();
}
