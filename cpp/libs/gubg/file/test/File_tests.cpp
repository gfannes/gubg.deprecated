#include "gubg/Testing.hpp"
#include "gubg/file/File.hpp"
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(FileTests);
    {
        TEST_TAG(Ctor);
        {
            File file;
            TEST_EQ("", file.name());
            TEST_EQ(File::Unknown, file.type());
        }
        {
            File file("abc");
            TEST_EQ("abc", file.name());
            TEST_EQ(File::Unknown, file.type());
        }
        {
            File file(string("abc"));
            TEST_EQ("abc", file.name());
            TEST_EQ(File::Unknown, file.type());
        }
        {
            File file("abc", File::Regular);
            TEST_EQ("abc", file.name());
            TEST_EQ(File::Regular, file.type());
        }
        {
            File file(string("abc"), File::Regular);
            TEST_EQ("abc", file.name());
            TEST_EQ(File::Regular, file.type());
        }
    }
    {
        TEST_TAG(Setters);
        File file;
        file.setName("abc");
        TEST_EQ("abc", file.name());
        TEST_EQ(File::Unknown, file.type());
        file.setType(File::Regular);
        TEST_EQ(File::Regular, file.type());
        file.setName("ABC").setType(File::Directory);
        TEST_EQ("ABC", file.name());
        file.setType(File::Directory);
    }
    {
        TEST_TAG(Append);
        File file;
        file << "a";
        TEST_EQ("a", file.name());
        file << "b";
        TEST_EQ("a/b", file.name());
        file << "/c";
        TEST_EQ("a/b/c", file.name());
        file << "";
        TEST_EQ("a/b/c", file.name());
    }
    {
        TEST_TAG(extension);
        File f("a/b/c");
        TEST_EQ("a/b/c", f.name());
        f.setExtension("obj");
        TEST_EQ("a/b/c.obj", f.name());
        f.setExtension("obj");
        TEST_EQ("a/b/c.obj", f.name());
        f.setExtension("obj2");
        TEST_EQ("a/b/c.obj2", f.name());
    }
    return 0;
}
#include "gubg/log/end.hpp"
