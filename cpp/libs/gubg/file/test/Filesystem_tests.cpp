//#define GUBG_LOG
#include "gubg/testing/Testing.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/l.hpp"
using namespace gubg::file;
using namespace std;

namespace 
{
    bool countNrFiles(size_t &count, const File &f)
    {
        MSS_BEGIN(bool, tree);
        //L("(" << f.type() << ") " << f.name());
        ++count;
        vector<File> files;
        MSS(ReturnCode::OK == read(files, f));
        for (auto ff: files)
            countNrFiles(count, ff);
        MSS_END();
    }
}

int main()
{
    TEST_TAG(FilesystemTests);
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
            const File home("/home");
            vector<File> files;
            TEST_OK(read(files, root));
            L("I found " << files.size() << " files in " << root.name());
            for (auto f: files)
                cout << f.name() << "(" << f.type() << "), ";
            cout << endl;

            {
                TEST_TAG(countNrFiles);
                size_t nrFiles = 0;
                countNrFiles(nrFiles, home);
                L("I found " << nrFiles << " files and dirs in " << home.name());
            }
            {
                TEST_TAG(Recursor_crtp);
                struct Counter: Recursor_crtp<Counter>
                {
                    size_t count;
                    Counter():count(0){}
                    void recursor_discoveredFile(const File &file)
                    {
                        ++count;
                    }
                };
                Counter counter;
                counter(home);
                L("Filecount: " << counter.count);
            }
        }
    }
    return 0;
}
