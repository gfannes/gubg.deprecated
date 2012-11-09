#ifndef HEADER_gubg_testing_TestMgr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_testing_TestMgr_hpp_ALREADY_INCLUDED

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
        enum class TestResult {OK, Failure};
        struct Statistics
        {
            Statistics();
            void addResult(TestResult);
            Statistics &operator+=(const Statistics &rhs);
            unsigned int nrTotal() const;

            unsigned int nrOK;
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
                virtual ~TestMaster();
                static TestMaster &instance();

                void report(const TestTag::ThreadStats &root);

                enum class ReportType {Full, OnlyErrors};
                const static ReportType defaultReportType = ReportType::OnlyErrors;
                void set(ReportType reportType){reportType_ = reportType;}

            private:
                friend std::ostream &::operator<<(std::ostream &, const TestMaster &);
                TestMaster();
                TestMaster(const TestMaster &);
                TestMaster &operator=(const TestMaster &);

                typedef std::list<TestTag::ThreadStats> ThreadStatss;
                ThreadStatss threadStatss_;
                ReportType reportType_;
        };
    }
}

#endif
