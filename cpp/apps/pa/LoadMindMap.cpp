#include "pa/LoadMindMap.hpp"
#include "pa/Model.hpp"
#include "gubg/parse/xml/Parser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include <string>
#include <fstream>
#include <map>
using namespace pa;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "LoadMindMap"
#include "gubg/log/begin.hpp"
namespace pa
{
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
		    MSS(n != attrs.end() && v != attrs.end());
		    if (n->second == value)
		    {
			    MSS(!location.empty());
			    auto o = attrs.find("OBJECT");
			    if (o != attrs.end())
			    {
				    gubg::Strange strange(o->second);
				    MSS(strange.popString("org.freeplane.features.format.FormattedNumber|"));
				    MSS(strange.popFloat(location.back()->value));
			    }
			    else
			    {
				    gubg::Strange strange(v->second);
				    MSS(strange.popFloat(location.back()->value));
			    }
			    L("Detected " << value << " for " << location.back()->desc << ": " << location.back()->value);
		    }
		    else if (n->second == fraction)
		    {
			    MSS(!location.empty());
			    auto o = attrs.find("OBJECT");
			    if (o != attrs.end())
			    {
				    gubg::Strange strange(o->second);
				    MSS(strange.popString("org.freeplane.features.format.FormattedNumber|"));
				    MSS(strange.popFloat(location.back()->fraction));
			    }
			    else
			    {
				    gubg::Strange strange(v->second);
				    MSS(strange.popFloat(location.back()->fraction));
			    }
			    L("Detected " << fraction << " for " << location.back()->desc << ": " << location.back()->fraction);
		    }
		    else
		    {
			    MSS(!location.empty());
			    location.back()->attributes[n->second] = v->second;
			    L("Detected " << n->second << " for " << location.back()->desc << ": " << v->second);
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
}
pa::ReturnCode LoadMindMap::execute(const Options &options)
{
	MSS_BEGIN(ReturnCode, "Loading mindmap: " << STREAM(options.input, options.value, options.fraction, options.fraction_default));

	string xml;
	MSS(read(xml, options.input));

	double defaultFraction = 1.0;
	if (!options.fraction_default.empty())
	{
		gubg::Strange strange(options.fraction_default);
		MSS(strange.popFloat(defaultFraction));
	}

    Parser p(model(), options.value, options.fraction, defaultFraction);
    MSS(p.process(xml));

	gubg::tree::dfs::iterate(model(), Aggregate());

	MSS_END();
}
