#ifndef HEADER_vix_model_hpp_ALREADY_INCLUDED
#define HEADER_vix_model_hpp_ALREADY_INCLUDED

#include "codes.hpp"
#include <string>

class Model
{
    public:
        ReturnCode add(char ch) { buffer_.push_back(ch); }

        std::string get() const {return buffer_;}

    private:
        std::string buffer_;
};

#endif
