#ifndef vix_model_hpp
#define vix_model_hpp

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
