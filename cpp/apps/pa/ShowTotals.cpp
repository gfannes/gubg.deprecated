#include "pa/ShowTotals.hpp"
#include "gubg/parse/xml/Parser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include "gubg/planning/Planning.hpp"
#include <string>
#include <fstream>
#include <map>
using namespace pa;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "ShowTotals"
#include "gubg/log/begin.hpp"

namespace pa
{
    struct Node
    {
        typedef string Name;
        typedef double Value;
        typedef double Fraction;
        typedef vector<Node> Childs;
        typedef string Id;
	typedef map<string, string> Attributes;

        Name desc;
        Value value;
        Fraction fraction;
        Childs childs;
        Id id;
	Attributes attributes;

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
		    MSS(n != attrs.end() && v != attrs.end());
		    if (n->second == value)
		    {
			    auto o = attrs.find("OBJECT");
			    MSS(n != attrs.end() && v != attrs.end() && o != attrs.end());
			    MSS(!location.empty());
			    gubg::Strange strange(o->second);
			    MSS(strange.popString("org.freeplane.features.format.FormattedNumber|"));
			    MSS(strange.popFloat(location.back()->value));
			    L("Detected " << value << " for " << location.back()->desc << ": " << location.back()->value);
		    }
		    else if (n->second == fraction)
		    {
			    auto o = attrs.find("OBJECT");
			    MSS(n != attrs.end() && v != attrs.end() && o != attrs.end());
			    MSS(!location.empty());
			    gubg::Strange strange(o->second);
			    MSS(strange.popString("org.freeplane.features.format.FormattedNumber|"));
			    MSS(strange.popFloat(location.back()->fraction));
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
    struct Planner
    {
	gubg::planning::Planning planning;
	gubg::planning::Line *line;
	std::ostringstream error_;

	Planner()
	{
		using namespace gubg::planning;
		planning.resources.addWorker("gfa", 0.8);
		planning.resources.addWorker("wba", 0.5);
		for (auto d: workDays(90))
			planning.resources.addDay(d);
		for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 20)))
			planning.resources.absence("gfa", d);
		for (auto d: dayRange(Day(2013, 7, 1), Day(2013, 7, 12)))
			planning.resources.absence("wba", d);

		Workers workers;
		workers.push_back("gfa");
		planning.getLine("XX").setMaxSweatPerDay(1.0).setWorkers(workers);
		workers.push_back("wba");
		planning.getLine("BLITS_L").setMaxSweatPerDay(2.0).setWorkers(workers);
		planning.getLine("BLITS_S").setMaxSweatPerDay(2.0).setWorkers(workers);
	}

	bool run()
	{
		if (!error_.str().empty())
		{
			std::cout << error_.str();
			return false;
		}
		return planning.run();
	}

	template <typename Path>
		bool open(Node &n, Path &p)
		{
			SS(n.desc, p.size());
			const auto total = n.value*n.fraction;
			if (total <= 0)
				return false;
			switch (p.size())
			{
				case 0:
					break;
				case 1:
					line = &planning.getLine(n.desc);
					break;
				case 2:
					{
						gubg::planning::Task task(n.desc, total);
						auto deadline = n.attributes.find("deadline");
						if (deadline != n.attributes.end())
						{
							L("Found deadline attribute");
							auto dl = gubg::planning::Day(deadline->second);
							if (!dl.isValid())
								error_ << "Could not parse deadline attribute for node " << n.desc << ": \"" << deadline->second << "\"" << std::endl;
							else
								task.deadline = dl;
						}
						line->addTask(task);
					}
					break;
				default: return false; break;
			}
			return true;
		}

	template <typename Path>
		void close(Node &n, Path &p) const { }
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
	Planner planner;
	gubg::tree::dfs::iterate(root, planner);
	MSS(planner.run());
	planner.planning.stream(cout);
	if (!options.output.name().empty())
	{
		ofstream fo(options.output.name());
		planner.planning.stream(fo);
	}

	MSS_END();
}
