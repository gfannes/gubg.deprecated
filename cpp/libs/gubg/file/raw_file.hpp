#ifndef gubg_file_raw_file_hpp
#define gubg_file_raw_file_hpp

#include "gubg/tree/raw.hpp"
#include <string>

#include <iostream>
#define LOG(msg) std::cout << msg << std::endl

namespace gubg
{
    namespace file
    {
        namespace raw
        {
            using namespace std;
            using namespace gubg::tree::raw;

            enum class ReturnCode {MSS_DEFAULT_CODES};

            //The data we keep for a file or directory node
            struct Data
            {
                ~Data(){LOG("Data::dtor " << name);}
                string name;
            };

            enum ExpandStrategy {Shallow, Recursive};

            struct Directory: Node<Data>
            {
                string path() const;
                ReturnCode expand(ExpandStrategy);
            };
            struct Regular: Node<Data>
            {
                string filename() const;
            };
        }
    }
}

#endif
