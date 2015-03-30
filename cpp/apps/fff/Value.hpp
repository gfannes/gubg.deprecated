#ifndef HEADER_fff_Value_hpp_ALREADY_INCLUDED
#define HEADER_fff_Value_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include <string>
#include <ostream>

namespace fff { 
    class Value
    {
        public:
            Value(const gubg::file::File &);
            Value(std::string);

            bool operator<(const Value &) const;

            //Will convert a string or file, ideal for CLI args
            gubg::file::File as_file() const;
            std::string as_string() const;

            void stream(std::ostream &) const;

        private:
            std::string str_;
    };
    inline std::ostream &operator<<(std::ostream &os, const Value &v) {v.stream(os); return os;}
} 

#endif
