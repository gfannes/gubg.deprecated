#include "file/File.hpp"
#include "testing/Testing.hpp"
#define GUBG_LOG
#include "logging/Log.hpp"
//#include "dir.hpp"

#include <iostream>

using namespace std;
using namespace gubg;

namespace
{
    const string thisFilepath(__FILE__);
    const string unexistingFilepath("/this/file/does/not/exist.txt");
#ifdef __linux
    const string homeDir("/home/gfannes");
#else
    const string homeDir("c:/home/gfa");
#endif
}

int main(int argc, char **argv)
{
    TEST_TAG(main);
    {
        TEST_TAG(File);
        LOG_S(FileTests);
        {
            TEST_TAG(absolute);
            auto file = file::Regular::create(thisFilepath);
            TEST_TRUE(file);
            TEST_EQ("test.cpp", file->name());
            TEST_EQ(thisFilepath, file->filepath());
            TEST_EQ(".cpp", file->extension());
            TEST_TRUE(file->exists());
            TEST_TRUE(file->isAbsolute());
            TEST_FALSE(file->isRelative());
        }
        {
            TEST_TAG(relative);
            const string relativeFilepath("test/test.cpp");
            auto file = file::Regular::create(relativeFilepath);
            TEST_TRUE(file);
            TEST_EQ("test.cpp", file->name());
            TEST_EQ(relativeFilepath, file->filepath());
            TEST_TRUE(file->exists());
            TEST_FALSE(file->isAbsolute());
            TEST_TRUE(file->isRelative());
        }
        {
            TEST_TAG(load);
            string content;
            TEST_FALSE(file::Regular::create(unexistingFilepath)->load(content));
            TEST_TRUE(file::Regular::create(thisFilepath)->load(content));
            LOG_SM(load, "I loaded " << content.size() << " bytes from " << thisFilepath);
        }
    }
    {
        TEST_TAG(Directory);
        LOG_S(DirectoryTests);
        auto dir = file::Directory::create(homeDir);
        auto nr = file::Directory::expand(dir, file::ExpandStrategy::Shallow);
        LOG_M("I found " << nr << " files in " << dir->path() << " using Shallow");
        nr = file::Directory::expand(dir, file::ExpandStrategy::Recursive);
        LOG_M("I found " << nr << " files in " << dir->path() << " using Recursive");
#if 0
    {
        Dir dir("/home/gfannes/data/raw");
        cout << dir.origDir() << endl;
        cout << dir.toString() << endl;
        vector<string> files;
        vector<string> dirs;
        int i;
        dir.entries(&files,&dirs);
        for (i=0;i<files.size();i++)
            cout << "file " << i << " = " << files[i] << "" << endl;
        for (i=0;i<dirs.size();i++)
            cout << "dir " << i << " = " << dirs[i] << "" << endl;
    }
    Dir dir2("./");
    cout << "" << dir2.toString() << "" << endl;
#endif
    }
    return 0;
}
