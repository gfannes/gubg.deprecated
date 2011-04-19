#include "Exit.hpp"
#include "parsing/Parser.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/file.hpp"
#include <map>
#include <list>
#include <iostream>
#include <fstream>
using namespace std;
using namespace gubg;

#define OUTPUT(msg) if (switches["verbose"]) std::cout << msg

int main(int argc, char **argv)
{
    map<string, bool> switches;
    list<string> filenames;
    {
        map<string, string> values;
        {
            OptionParser op("Meta data compilation tool");
            op.addSwitch("-h", "--help", "Print this help", [&op](){meta::finalize(op.help(), meta::Status::Ok);});
            switches["verbose"] = false;
            op.addSwitch("-v", "--verbose", "Verbose mode", [&switches](){switches["verbose"] = true;});
            op.addOptional("-i", "--input FILE", "FILE with filenames to process", [&values](string filename){values["input"] = filename;});
            op.addOptional("-f", "--file FILE", "FILE to process", [&filenames](string filename){filenames.push_back(filename);});
            auto args = OptionParser::convertArgs(argc, argv);
            if (!op.parse(args))
                meta::finalize("Failed to parse the arguments");
            if (!args.empty())
            {
                ostringstream oss;
                oss << "Could not process following arguments:";
                for (auto arg: args)
                    oss << " " << arg;
                meta::finalize(oss.str());
            }
        }

        //Append the filenames specified in values["input"] to filenames
        if (values.count("input"))
        {
            ifstream fi(values["input"]);
            string filename;
            while (fi.good())
            {
                getline(fi, filename);
                filenames.push_back(filename);
            }
        }
    }

    //Print the files that will be parsed
    OUTPUT("Filenames that will be processed:" << endl);
    for (auto fn: filenames)
        OUTPUT(fn << endl);

    //Process the selected files
    meta::Parser parser;
    for (auto filename: filenames)
    {
        OUTPUT("Processing " << filename << endl);
        std::string code;
        if (!file::readContent(code, filename))
            meta::finalize("Could not read the file");

        auto structure = parser.parse(code);
        OUTPUT(structure->toString() << endl);
        auto cpn = structure->countPerName();
        for (auto nc: cpn)
            OUTPUT("\t" << nc.first << ": " << nc.second << endl);
    }
    OUTPUT("All files are processed." << endl);
    return 0;
}
