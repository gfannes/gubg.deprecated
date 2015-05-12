#include "catch/catch.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    bool countNrFiles(size_t &count, const File &f)
    {
        MSS_BEGIN(bool, f);
        //L("(" << f.type() << ") " << f.name());
        ++count;
        vector<File> files;
        MSS(ReturnCode::OK == read(files, f));
        for (auto ff: files)
            if (isDirectory(ff))
                countNrFiles(count, ff);
        MSS_END();
    }
}

TEST_CASE("FilesystemTests", "[ut]")
{
    S();
    SECTION("resolve")
    {
        File file(".");
        REQUIRE(MSS_IS_OK(resolve(file)));
        L(file);
    }
    SECTION("getcwd")
    {
        L(gubg::file::getcwd().name());
    }
    SECTION("read")
    {
        SECTION("regular")
        {
            const File f(__FILE__);
            size_t s;
            REQUIRE(MSS_IS_OK(size(s, f)));
            L("File size for " << f.name() << ": " << s << " bytes");
            string content;
            REQUIRE(MSS_IS_OK(read(content, f)));
            L(content);
        }
#if 0
        SECTION("directory")
        {
            const File root("/");
            const File home("/home/gfannes");
            const File gubg("/home/gfannes/gubg");
            vector<File> files;
            REQUIRE(MSS_IS_OK(read(files, home)));
            L("I found " << files.size() << " files in " << home.name());
            for (auto f: files)
                cout << f.name() << "(" << f.type() << "), ";
            cout << endl;

            SECTION("countNrFiles")
            {
                size_t nrFiles = 0;
                countNrFiles(nrFiles, gubg);
                L("I found " << nrFiles << " files and dirs in " << gubg.name());
            }
            SECTION("recurse")
            {
                struct Counter
                {
                    size_t count;
                    Counter():count(0){}
                    ReturnCode recursor_discoveredFile(const File &file)
                    {
                        MSS_BEGIN(ReturnCode, file);
                        ++count;
                        //MSS(count < 10, Stop);
                        //MSS(count < 10, Skip);
                        MSS_END();
                    }
                };
                Counter counter;
                recurse(counter, gubg);
                L("Filecount: " << counter.count);
            }
        }
#endif
    }
    SECTION("remove")
    {
        File file("remove_me.txt");
        REQUIRE(MSS_IS_OK(write("abc", file)));
        REQUIRE(MSS_IS_OK(remove(file)));
        REQUIRE(!MSS_IS_OK(remove(file)));
    }
    SECTION("copy")
    {
        File file("remove_me.txt");
        REQUIRE(MSS_IS_OK(write("abc", file)));
        File file2("remove_me_2.txt");
        REQUIRE(MSS_IS_OK(copy(file, file2)));
        REQUIRE(MSS_IS_OK(remove(file)));
        REQUIRE(MSS_IS_OK(remove(file2)));
    }
    SECTION("mkdir")
    {
        File dir("test_dir/aaa");
        if (isDirectory(dir))
        {
            REQUIRE(MSS_IS_OK(remove(dir)));
        }
        REQUIRE(!isDirectory(dir));
        REQUIRE(MSS_IS_OK(mkdir(dir)));
        REQUIRE(isDirectory(dir));
        //remove(dir);
    }
}
#include "gubg/log/end.hpp"
