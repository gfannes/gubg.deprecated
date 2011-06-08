#include "file/File.hpp"
#include "testing/Testing.hpp"
//#include "dir.hpp"

using namespace std;
using namespace gubg;

namespace
{
    const string thisFilename(__FILE__);
}

int main()
{
    {
        TEST_TAG(File);
        {
            TEST_TAG(absolute);
            auto file = file::Regular::create(thisFilename);
            TEST_TRUE(file);
            TEST_EQ("test.cpp", file->name());
            TEST_EQ(thisFilename, file->filename());
            TEST_TRUE(file->exists());
            TEST_TRUE(file->isAbsolute());
            TEST_FALSE(file->isRelative());
        }
        {
            TEST_TAG(relative);
            const string relativeFilename("test/test.cpp");
            auto file = file::Regular::create(relativeFilename);
            TEST_TRUE(file);
            TEST_EQ("test.cpp", file->name());
            TEST_EQ(relativeFilename, file->filename());
            TEST_TRUE(file->exists());
            TEST_FALSE(file->isAbsolute());
            TEST_TRUE(file->isRelative());
        }
    }
    {
        TEST_TAG(Directory);
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
