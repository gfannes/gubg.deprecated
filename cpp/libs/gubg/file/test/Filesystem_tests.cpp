#include "gubg/Testing.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "test"
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

int main()
{
    TEST_TAG(FilesystemTests);
    {
        TEST_TAG(resolve);
        File file(".");
        TEST_OK(resolve(file));
        L(file);
    }
    {
        TEST_TAG(getcwd);
        L(gubg::file::getcwd().name());
    }
    {
        TEST_TAG(read);
        {
            TEST_TAG(regular);
            const File f(__FILE__);
            size_t s;
            TEST_OK(size(s, f));
            L("File size for " << f.name() << ": " << s << " bytes");
            string content;
            TEST_OK(read(content, f));
            L(content);
        }
        {
            TEST_TAG(directory);
            const File root("/");
            const File home("/home/gfannes");
            const File gubg("/home/gfannes/gubg");
            vector<File> files;
            TEST_OK(read(files, home));
            L("I found " << files.size() << " files in " << home.name());
            for (auto f: files)
                cout << f.name() << "(" << f.type() << "), ";
            cout << endl;

            {
                TEST_TAG(countNrFiles);
                size_t nrFiles = 0;
                countNrFiles(nrFiles, gubg);
                L("I found " << nrFiles << " files and dirs in " << gubg.name());
            }
            {
                TEST_TAG(recurse);
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
    }
    {
        TEST_TAG(remove);
        File file("remove_me.txt");
        TEST_OK(write("abc", file));
        TEST_OK(remove(file));
        TEST_KO(remove(file));
    }
    {
        TEST_TAG(copy);
        File file("remove_me.txt");
        TEST_OK(write("abc", file));
        File file2("remove_me_2.txt");
        TEST_OK(copy(file, file2));
        TEST_OK(remove(file));
        TEST_OK(remove(file2));
    }
    return 0;
}
#include "gubg/log/end.hpp"
