#include "pa/ShowTotals.hpp"
#include "gubg/parse/xml/Parser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include <string>
#include <fstream>
using namespace pa;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "ShowTotals"
#include "gubg/log/begin.hpp"

namespace pa
{
    struct Node
    {
        typedef string Name;
        typedef double Value;
        typedef double Fraction;
        typedef std::vector<Node> Childs;
        typedef string Id;

        Name desc;
        Value value;
        Fraction fraction;
        Childs childs;
        Id id;

        Node():value(), fraction(){}
    };
    struct Parser: gubg::xml::Parser_crtp<Parser>
    {
        Node &root;
        typedef std::vector<Node*> Location;
        Location location;
        const string value;
        const string fraction;
        const double defaultFraction;
        Parser(Node &r, string n, string f, double df):root(r), value(n), fraction(f), defaultFraction(df){}

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
                location.back()->fraction = defaultFraction;
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
                auto id = attrs.find("ID");
                MSS(it != attrs.end() && id != attrs.end());
                L(it->second);
                location.back()->desc = it->second;
                location.back()->id = id->second;
            }
            if (path.back() == "attribute")
            {
                auto n = attrs.find("NAME");
                auto v = attrs.find("VALUE");
                auto o = attrs.find("OBJECT");
                MSS(n != attrs.end() && v != attrs.end() && o != attrs.end());
                if (n->second == value)
                {
                    MSS(!location.empty());
                    gubg::Strange strange(o->second);
                    MSS(strange.popString("org.freeplane.features.format.FormattedNumber|"));
                    MSS(strange.popFloat(location.back()->value));
                    L("Detected " << value << " for " << location.back()->desc << ": " << location.back()->value);
                }
                if (n->second == fraction)
                {
                    MSS(!location.empty());
                    gubg::Strange strange(o->second);
                    MSS(strange.popString("org.freeplane.features.format.FormattedNumber|"));
                    MSS(strange.popFloat(location.back()->fraction));
                    L("Detected " << fraction << " for " << location.back()->desc << ": " << location.back()->fraction);
                }
            }
            MSS_END();
        }
    };
}
namespace 
{
    struct Aggregate
    {
        template <typename Path>
            bool open(Node &n, Path &p) const
            {
                return true;
            }
        template <typename Path>
            void close(Node &n, Path &p) const
            {
                //We add the value of n to the parent, if any
                if (!p.empty())
                    p.back()->value += n.value*n.fraction;
            }
    };
    struct Show
    {
        ostream &os;
        Show(ostream &o):os(o){}
        template <typename Path>
            bool open(Node &n, Path &p) const
            {
                const auto total = n.value*n.fraction;
                if (total <= 0)
                    return false;
                os << string(p.size()*2, ' ') << n.desc << " (" << total << ": " << n.fraction << " * " << n.value << ")" << endl;
                return true;
            }
        template <typename Path>
            void close(Node &n, Path &p) const
            {
            }
    };
    struct TJ
    {
        string prevId;
        ostream &os;
        TJ(ostream &o):os(o){}
        template <typename Path>
            bool open(Node &n, Path &p)
            {
                const auto total = n.value*n.fraction;
                if (total <= 0)
                    return false;
                switch (p.size())
                {
                    case 0:
                        break;
                    case 1:
                        prevId.clear();
                        os << indent(p.size()) << "task " << n.desc << " \"" << n.desc << "\"" << endl << indent(p.size()) << "{" << endl;
                        break;
                    case 2:
                        os << indent(p.size()) << "task " << n.id << " \"" << n.desc << "\"" << endl << indent(p.size()) << "{" << endl;
                        os << indent(p.size()+1) << "effort " << total << "d";
                        if (!prevId.empty())
                            os <<  " depends !" << prevId;
                        os << endl;
                        prevId = n.id;
                        break;
                    default:
                        return false;
                        break;
                }
                return true;
            }
        template <typename Path>
            void close(Node &n, Path &p) const
            {
                switch (p.size())
                {
                    case 0:
                        break;
                    case 1:
                    case 2:
                        os << indent(p.size()) << "}" << endl;
                        break;
                }
            }
        static string indent(size_t level) {return string(2*(level-1), ' ');}
    };
}
pa::ReturnCode ShowTotals::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode, "Showing totals: " << STREAM(options.input, options.value, options.fraction, options.fraction_default));

    string xml;
    MSS(read(xml, options.input));

    double defaultFraction = 1.0;
    if (!options.fraction_default.empty())
    {
        gubg::Strange strange(options.fraction_default);
        MSS(strange.popFloat(defaultFraction));
    }

    Node root;
    {
        Parser p(root, options.value, options.fraction, defaultFraction);
        MSS(p.process(xml));
    }

    gubg::tree::dfs::iterate(root, Aggregate());
    gubg::tree::dfs::iterate(root, Show(cout));
    if (!options.output.name().empty())
    {
        ofstream fo(options.output.name());
        TJ tj(fo);
        gubg::tree::dfs::iterate(root, tj);
    }

    MSS_END();
}
