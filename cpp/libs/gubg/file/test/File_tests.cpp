#include "gubg/testing/Testing.hpp"
#include "gubg/file/File.hpp"
using namespace gubg::file;
using namespace std;

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
    return 0;
}
