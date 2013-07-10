#ifndef HEADER_gubg_msgpack_test_Helper_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_test_Helper_hpp_ALREADY_INCLUDED

#include <string>

namespace helper
{
    using namespace std;
    string str_(const vector<int> &bytes)
    {
        string str;
        for (auto byte: bytes)
            str.push_back((char)byte);
        return str;
    }
}

#endif
