#ifndef gubg_testing_Testing_hpp
#define gubg_testing_Testing_hpp

#include <memory>
#include <string>
#include <ostream>
#include <iostream>
#include <map>
#include <vector>

#include <sstream>

namespace gubg
{
    struct SourceLocation;
}
std::ostream &operator<<(std::ostream &os, const gubg::SourceLocation &location);

namespace gubg
{
    struct TestTag
    {
        TestTag(){}
        TestTag(const std::string &s, const std::string &c, const std::string &p):
            testSuite(s), testCase(c), testPart(p){}
        std::string testSuite;
        std::string testCase;
        std::string testPart;
    };
    class TestMgr;
    class TestTagger;
    typedef std::shared_ptr<TestMgr> TestMgrPtr;
    class TestMgr
    {
        public:
            static TestMgr &instance();
            ~TestMgr();

            void success();
            void failure();

        private:
            TestMgr();

            friend class TestTagger;
            //Returns the current tag
            TestTag setTestTag(TestTag tag);

            struct Statistics
            {
                Statistics():
                    nrSuccess(0),
                    nrFailure(0){}
                unsigned int total() const {return nrSuccess + nrFailure;}
                unsigned int nrSuccess;
                unsigned int nrFailure;
            };
            TestTag currentTag_;
            //Set currentTag_ and then call syncCurrentStatistics_() to set currentStats_
            void syncCurrentStatistics_();
            Statistics *currentStats_;
            typedef std::map<std::string, Statistics> StatsPerPart;
            typedef std::map<std::string, StatsPerPart> StatsPerCase;
            typedef std::map<std::string, StatsPerCase> StatsPerSuite;
            StatsPerSuite statsPerSuite_;
            typedef std::vector<std::string> Order;
            Order suiteOrder_;
            typedef std::map<std::string, Order> CaseOrderPerSuite;
            CaseOrderPerSuite caseOrderPerSuite_;
            typedef std::pair<std::string, std::string> SuiteCase;
            typedef std::map<SuiteCase, Order> PartOrderPerSuiteCase;
            PartOrderPerSuiteCase partOrderPerSuiteCase_;
    };

    //RAII class to tag some part of a test
    struct TestTagger
    {
        TestTagger(const std::string &testSuite, const std::string &testCase = "", const std::string &testPart = "")
        {
            prevTag_ = TestMgr::instance().setTestTag(TestTag(testSuite, testCase, testPart));
        }
        virtual ~TestTagger()
        {
            TestMgr::instance().setTestTag(prevTag_);
        }
        
        private:
        TestTag prevTag_;
    };
#define TEST_SUITE(suite) \
    const std::string l_gubg_test_suite_name_(#suite); \
    gubg::TestTagger l_gubg_test_tagger_suite_(l_gubg_test_suite_name_)
#define TEST_CASE(testCase) \
    const std::string l_gubg_test_case_name_(#testCase); \
    gubg::TestTagger l_gubg_test_tagger_case_(l_gubg_test_suite_name_, l_gubg_test_case_name_)
#define TEST_PART(part) \
    const std::string l_gubg_test_part_name_(#part); \
    gubg::TestTagger l_gubg_test_tagger_part_(l_gubg_test_suite_name_, l_gubg_test_case_name_, l_gubg_test_part_name_)
#define TEST_TAG(suite, testCase, part) \
    gubg::TestTagger l_gubg_test_tagger_tag_(suite, testCase, part)
    
    struct SourceLocation
    {
        SourceLocation(const std::string &f, unsigned int l):
            filename(f),
            lineNr(l){}

        std::string filename;
        unsigned int lineNr;
    };
#define HERE() gubg::SourceLocation(__FILE__, __LINE__)

    namespace
    {
        template <typename T>
        std::string toString_(const T &t)
        {
            std::ostringstream oss;
            oss << &t;
            return oss.str();
        }
    }
    template<typename ExpectedT, typename ActualT>
        void test_eq(const ExpectedT &expected, const ActualT &actual, SourceLocation location)
        {
            if (actual == expected)
                TestMgr::instance().success();
            else
            {
                TestMgr::instance().failure();
                std::cout << location << ": " << toString_(expected) << " was expected, but " << toString_(actual) << " was received." << std::endl;
            }
        }
#define TEST_EQ(expected, actual) gubg::test_eq(expected, actual, HERE())

    template<typename NotExpectedT, typename ActualT>
        void test_neq(const NotExpectedT &notExpected, const ActualT &actual, SourceLocation location)
        {
            if (actual != notExpected)
                TestMgr::instance().success();
            else
            {
                TestMgr::instance().failure();
                std::cout << location << ": " << notExpected << " was not expected, but it was received." << std::endl;
            }
        }
#define TEST_NEQ(notExpected, actual) gubg::test_neq(notExpected, actual, HERE())

    template<typename T>
        void test_true(const T &v, SourceLocation location)
        {
            if (v)
                TestMgr::instance().success();
            else
            {
                TestMgr::instance().failure();
                std::cout << location << ": it should be true, but it is false." << std::endl;
            }
        }
#define TEST_TRUE(value) gubg::test_true(value, HERE())

    template<typename T>
        void test_false(const T &v, SourceLocation location)
        {
            if (!v)
                TestMgr::instance().success();
            else
            {
                TestMgr::instance().failure();
                std::cout << location << ": it should be false, but it is true." << std::endl;
            }
        }
#define TEST_FALSE(value) gubg::test_false(value, HERE())
}

#endif
