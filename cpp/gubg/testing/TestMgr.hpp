#ifndef gubg_testing_TestMgr_hpp
#define gubg_testing_TestMgr_hpp

#include <string>
#include <vector>
#include <map>
#include <list>
#include <ostream>

namespace gubg
{
    namespace testing
    {
        class TestMaster;
    }
}
std::ostream &operator<<(std::ostream &, const gubg::testing::TestMaster &);

namespace gubg
{
    namespace testing
    {
        enum class TestResult {Success, Failure};
        struct Statistics
        {
            Statistics();
            void addResult(TestResult);
            Statistics &operator+=(const Statistics &rhs);
            unsigned int nrTotal() const;

            unsigned int nrSuccess;
            unsigned int nrFailure;
        };

        //Thread-local instances that handle test statistics per thread
        class TestTag
        {
            public:
                //Variables only used by the root
                struct ThreadStats
                {
                    ThreadStats();
                    void addTag(const std::string &tag);
                    void popTag();
                    void addResult(TestResult result);

                    typedef std::vector<std::string> Tags;
                    Tags tags_;
                    typedef std::map<Tags, Statistics> StatsPerTags;
                    StatsPerTags statsPerTags_;
                    Statistics *currentStats_;
                };

                TestTag(const std::string &tag);
                virtual ~TestTag();

                void addResult(TestResult);

            private:
                TestTag(const TestTag &);
                TestTag &operator=(const TestTag &);

                std::string tag_;

                ThreadStats *root_;
        };

        //Singleton that handles all TestTag instances and global statistics reporting
        class TestMaster
        {
            public:
                static TestMaster &instance();

                void report(const TestTag::ThreadStats &root);

            private:
                friend std::ostream &::operator<<(std::ostream &, const TestMaster &);
                TestMaster(){}
                TestMaster(const TestMaster &);
                TestMaster &operator=(const TestMaster &);

                typedef std::list<TestTag::ThreadStats> ThreadStatss;
                ThreadStatss threadStatss_;
        };
    }
}

#endif
