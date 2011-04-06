#ifndef gubg_testing_Testing_hpp
#define gubg_testing_Testing_hpp

#include "testing/TestMgr.hpp"
#include <memory>
#include <string>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

#include <sstream>

namespace gubg
{
    struct SourceLocation;
}
std::ostream &operator<<(std::ostream &os, const gubg::SourceLocation &location);

namespace gubg
{
    template <typename T>
    std::string toHex(const std::basic_string<T> &binary)
    {
        std::ostringstream res;
        res << std::hex;
        for (auto byte: binary)
            res << "0x" << std::setw(2) << std::setfill('0') << (int)(0xff & byte) << ", ";
        return res.str();
    }
    template <typename T1, typename T2>
        bool areEqual(const T1 &lhs, const T2 &rhs)
        {
            return lhs == rhs;
        }
    template <typename T1, typename T2>
        bool areEqual(const std::basic_string<T1> &lhs, const std::basic_string<T2> &rhs)
        {
            if (sizeof(T1) != sizeof(T2) || lhs.size() != rhs.size())
                return false;
            const T1 *t1 = lhs.data();
            const T2 *t2 = rhs.data();
            if (nullptr == t1 && nullptr == t2)
                return true;
            if (0 == std::memcmp((void*)t1, (void*)t2, sizeof(T1)*lhs.size()))
                return true;
            return false;
        }
    template <>
        bool areEqual<char, char>(const std::string &lhs, const std::string &rhs);
    template <>
        bool areEqual<unsigned char, unsigned char>(const std::basic_string<unsigned char> &lhs, const std::basic_string<unsigned char> &rhs);

//Uncomment this to get a verbose version of TEST_TAG to get an indication where things go wrong
//#define L_TESTING_VERBOSE
#ifdef L_TESTING_VERBOSE
#define TEST_TAG(tag) \
    gubg::testing::TestTag l_gubg_testing_test_tag_(#tag); \
    std::cout << #tag << std::endl
#else
#define TEST_TAG(tag) \
    gubg::testing::TestTag l_gubg_testing_test_tag_(#tag)
#endif

#define TEST_REPORT() std::cout << gubg::testing::TestMaster::instance() << std::endl
#define TEST_REPORT_TYPE(type) gubg::testing::TestMaster::instance().set(gubg::testing::TestMaster::ReportType::type)

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
        void test_eq(const ExpectedT &expected, const ActualT &actual, SourceLocation location, gubg::testing::TestTag &testTag)
        {
            bool success = areEqual(actual, expected);
            testTag.addResult(success ? gubg::testing::TestResult::Success : gubg::testing::TestResult::Failure);
            if (!success)
                std::cout << location << ": " << toString_(expected) << " was expected, but " << toString_(actual) << " was received." << std::endl;
        }
#define TEST_EQ(expected, actual) gubg::test_eq((expected), (actual), HERE(),l_gubg_testing_test_tag_)

    template<typename NotExpectedT, typename ActualT>
        void test_neq(const NotExpectedT &notExpected, const ActualT &actual, SourceLocation location, gubg::testing::TestTag &testTag)
        {
            bool success = (actual == notExpected);
            testTag.addResult(success ? gubg::testing::TestResult::Success : gubg::testing::TestResult::Failure);
            if (!success)
                std::cout << location << ": " << notExpected << " was not expected, but it was received." << std::endl;
        }
#define TEST_NEQ(notExpected, actual) gubg::test_neq((notExpected), (actual), HERE(), l_gubg_testing_test_tag_)

    template<typename T>
        void test_true(const T &v, SourceLocation location, gubg::testing::TestTag &testTag)
        {
            bool success = v;
            testTag.addResult(success ? gubg::testing::TestResult::Success : gubg::testing::TestResult::Failure);
            if (!success)
                std::cout << location << ": it should be true, but it is false." << std::endl;
        }
#define TEST_TRUE(value) gubg::test_true((value), HERE(), l_gubg_testing_test_tag_)

    template<typename T>
        void test_false(const T &v, SourceLocation location, gubg::testing::TestTag &testTag)
        {
            bool success = !v;
            testTag.addResult(success ? gubg::testing::TestResult::Success : gubg::testing::TestResult::Failure);
            if (!success)
                std::cout << location << ": it should be false, but it is true." << std::endl;
        }
#define TEST_FALSE(value) gubg::test_false((value), HERE(), l_gubg_testing_test_tag_)

#define TEST_THROW(ExceptionType, expr) \
    try \
    { \
        (expr); \
        l_gubg_testing_test_tag_.addResult(gubg::testing::TestResult::Failure) ; \
        std::cout << HERE() << ": an exception of class " #ExceptionType " was expected, but not received." << std::endl; \
    } \
    catch (ExceptionType &exc) \
    { \
        l_gubg_testing_test_tag_.addResult(gubg::testing::TestResult::Success) ; \
    }
}

#endif
