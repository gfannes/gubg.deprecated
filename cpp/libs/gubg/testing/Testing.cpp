#include "gubg/testing/Testing.hpp"
#include "gubg/Exception.hpp"
#include "gubg/OnlyOnce.hpp"
#include <iostream>
#include <sstream>
using namespace std;

std::ostream &operator<<(std::ostream &os, const gubg::testing::SourceLocation &location)
{
    return os << location.filename << ":" << location.lineNr;
}
std::ostream &operator<<(std::ostream &os, const std::nullptr_t &ptr)
{
    return os << "nullptr";
}

namespace
{
    template <typename T>
    string toStringValue_(const T &t)
    {
        ostringstream oss;
        oss << t;
        return oss.str();
    }
}

namespace gubg
{
    namespace testing
    {
        template <>
            std::string toString_<nullptr_t>(const nullptr_t &p){return "nullptr";}

        template <>
            bool areEqual<char, char>(const std::string &lhs, const std::string &rhs)
            {
                return lhs == rhs;
            }
        template <>
            bool areEqual<unsigned char, unsigned char>(const std::basic_string<unsigned char> &lhs, const std::basic_string<unsigned char> &rhs)
            {
                return lhs == rhs;
            }
        bool isPrintable(char ch)
        {
            if ('a' <= ch && ch <= 'z')
                return true;
            if ('A' <= ch && ch <= 'Z')
                return true;
            return false;
        }
        std::string toString_(const std::string &str)
        {
            std::ostringstream oss;
            std::ostringstream ossHex;
            OnlyOnce skipPipe;
            int nrHex = 0;
            oss << '"';
            for (auto it = str.begin(); it != str.end(); ++it)
            {
                char ch = *it;
                bool ip = isPrintable(ch);
                if (!ip)
                    ++nrHex;
                oss <<  (ip ? ch : '.');
                ossHex << (skipPipe() ? "" : "|") << std::hex << std::setw(2) << std::setfill('0') << (0xff&(unsigned int)ch);
            }
            oss << '"';
            if (nrHex)
                oss << " (" << ossHex.str() << ")";
            return oss.str();
        }
    }
}
