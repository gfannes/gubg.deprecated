#include "gubg/String.hpp"

namespace gubg { 

    void upcase(std::string &str)
    {
        for (auto &ch: str)
            ch = std::toupper(ch);
    }

} 
