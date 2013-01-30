#ifndef HEADER_da_Arduino_hpp_ALREADY_INCLUDED
#define HEADER_da_Arduino_hpp_ALREADY_INCLUDED

#include <string>

namespace da
{
    namespace arduino
    {
        void setModel(const std::string &);

        bool isUno();
        bool isMega();
    }
}

#endif
