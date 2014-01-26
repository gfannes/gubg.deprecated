//!@#adoc
//rip: recusive include processor
#include "rip/Codes.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/file/Forest.hpp"
#include "gubg/lua/State.hpp"
#include "gubg/hash/MD5.hpp"
#include "gubg/parse/cpp/Includes.hpp"
#include "lua.hpp"
#include <iostream>
#include <cstdlib>
#include <map>
#include <set>
#include <queue>
#include <sstream>
using namespace gubg;
using namespace gubg::hash;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "rip"
#include "gubg/log/begin.hpp"

namespace rip { 

    struct IncludeResolver
    {
        Forest forest;
        ReturnCode resolve(File &f, const string &inc)
        {
            MSS_BEGIN(ReturnCode);
            L(inc);
            MSS_Q(forest.resolve(f, inc));
            MSS_END();
        }
    };
    IncludeResolver includeResolver;

    typedef set<string> IncludeSet;
    struct IncludeExtractor: parse::cpp::Includes_crtp<IncludeExtractor>
    {
        void includes_detected(const string &str, parse::cpp::IncludeType type)
        {
            includeSet.insert(str);
        }
        IncludeSet includeSet;
    };

    ReturnCode extractIncludes(IncludeSet &includeSet, const File &f)
    {
        MSS_BEGIN(ReturnCode);
        IncludeExtractor extractor;
        MSS(extractor.process(f));
        extractor.includeSet.swap(includeSet);
        MSS_END();
    }

    typedef set<File> FileSet;
    class DependencyMgr
    {
        public:
            ReturnCode getDirectDependencies(FileSet &files, const File &src)
            {
                MSS_BEGIN(ReturnCode);
                L(src);
                files.clear();
                IncludeSet includeSet;
                MSS(extractIncludes(includeSet, src));
                L(STREAM(includeSet.size()));
                for (const auto &inc: includeSet)
                {
                    File f;
                    if (MSS_IS_OK(includeResolver.resolve(f, inc)))
                        files.insert(f);
                }
                L(STREAM(files.size()));
                MSS_END();
            }
            //src will be added to files as well
            ReturnCode getRecursiveDependencies(FileSet &files, const File &src)
            {
                MSS_BEGIN(ReturnCode);
                L(src);
                files.clear();
                typedef std::queue<File> Queue;
                Queue queue; queue.push(src);
                while (!queue.empty())
                {
                    const auto f = queue.front(); queue.pop();
                    if (files.count(f))
                        //We already have this file
                        continue;
                    FileSet directDeps;
                    MSS(getDirectDependencies(directDeps, f));
                    for (auto ff: directDeps)
                        queue.push(ff);
                    files.insert(f);
                }
                L(STREAM(files.size()));
                MSS_END();
            }
        private:
    };
    DependencyMgr dependencyMgr;

    struct Prerequisites
    {
        typedef std::map<File, MD5::Hash> HashPerFile;
        HashPerFile hashPerFile;

        string serialize() const
        {
            ostringstream oss;
            for (const auto &pair: hashPerFile)
            {
                oss << MD5::to_hex(pair.second) << " " << pair.first << endl;
            }
            return oss.str();
        }
    };

    class HashMgr
    {
        public:
            typedef MD5::Hash Hash;
            ReturnCode hash(Hash &h, const File &f) const
            {
                MSS_BEGIN(ReturnCode);
                string content;
                MSS(read(content, f));
                MD5 md5;
                md5 << content;
                h = md5.hash();
                MSS_END();
            }

        private:
    };
    HashMgr hashMgr;

    ReturnCode process(const File src, const File &dep)
    {
        MSS_BEGIN(ReturnCode);

        Prerequisites prereq;
        FileSet files;
        MSS(dependencyMgr.getRecursiveDependencies(files, src));
        for (const auto &f: files)
        {
            MSS(hashMgr.hash(prereq.hashPerFile[f], f));
        }

        const auto newContent = prereq.serialize();
        bool writeNewContent = false;
        string oldContent;
        switch (const auto rc = read(oldContent, dep))
        {
            case file::ReturnCode::OK:
                break;
            case file::ReturnCode::CouldNotOpenFile:
                L("Dependency file not found, I have to write a new version always");
                writeNewContent = true;
                break;
            default: MSS(rc); break;
        }
        if (!writeNewContent)
        {
            //We will compare old and new content, if they differ, we have to write the new content
            if (oldContent != newContent)
            {
                L("Old content is out of date, I will update it");
                writeNewContent = true;
            }
        }
        if (writeNewContent)
        {
            L("Writing dependency file");
            MSS(write(newContent, dep));
        }
        MSS_END();
    }

    extern "C"
    {
        //Called from lua, should have {src="", dep=""} on the stack
        int process_lua(lua_State *s)
        {
            S();
            if (!lua_istable(s, -1))
            {
                L("Expected a table as argument.");
                return 0;
            }
            auto getValue_ = [&](const char *key) -> File
            {
                lua_getfield(s, -1, key);
                luaL_checktype(s, -1, LUA_TSTRING);
                File ret(lua_tostring(s, -1));
                lua_pop(s, 1);
                return ret;
            };
            const auto rc = process(getValue_("src"), getValue_("dep"));
            if (!MSS_IS_OK(rc))
            {
                L("Failed to process: " << (int)rc);
            }
            return 0;
        }
    }

    struct Options
    {
        File input;
        vector<File> trees;
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
            op.addMandatory("-t", "--tree", "Add tree", [&](const string &fn){options.trees.push_back(fn);});
            MSS(op.parse(args));
        }

        cout << "options:" << endl << options << endl;
        MSS(!options.input.empty(), InputNotSpecified);
        string input_content;
        MSS(file::read(input_content, options.input), InputFileNotFound);

        for (auto tree: options.trees)
            includeResolver.forest.add(tree, {"cpp", "hpp"});

        auto ls = lua::State::create();
        MSS(ls.registerFunction(process_lua, "process"));
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
