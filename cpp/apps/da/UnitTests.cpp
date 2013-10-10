#include "da/UnitTests.hpp"
#include "da/CompileExe.hpp"
#include "gubg/file/Filesystem.hpp"
#include <regex>
#include <map>
using namespace da;
using gubg::file::File;
using namespace std;

#define GUBG_MODULE_ "UnitTests"
#include "gubg/log/begin.hpp"
namespace 
{
    struct AddCompileTasks
    {
        Tasks &tasks;
        map<Task_itf::Ptr, File> sourcePerTest;
        std::regex unitTest_re = std::regex(".*_tests\\.cpp");
        std::regex deprecated_re = std::regex(".*deprecated");
        AddCompileTasks(Tasks &t): tasks(t) {}
        ReturnCode recursor_discoveredFile(File f)
        {
            MSS_BEGIN(ReturnCode);
            if (isRegular(f))
            {
                L(f.name());
                if (regex_match(f.name(), unitTest_re))
                {
                    auto task = CompileExe::create(f.name(), ExeType::Debug);
                    tasks.push_back(task);
                    sourcePerTest[task] = f.name();
                }
            }
            else if (isDirectory(f))
            {
                MSS(!regex_match(f.name(), deprecated_re), Skip);
            }
            MSS_END();
        }
    };
}
ReturnCode UnitTests::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode);
    Tasks unitTests;
    AddCompileTasks addCompileTasks(unitTests);
    gubg::file::recurse(addCompileTasks);
    verbose("I found", unitTests.size(), "unit tests");
    vector<File> failedTests;
    for (auto unitTest: unitTests)
    {
        if (!MSS_IS_OK(unitTest->execute(options)))
            failedTests.push_back(addCompileTasks.sourcePerTest[unitTest]);
    }
    verbose(failedTests.size(), "/", unitTests.size(), "tests FAILED");
    for (auto ft: failedTests)
        verbose("\t", ft.name());
    MSS(failedTests.empty());
    MSS_END();
}
#include "gubg/log/end.hpp"

