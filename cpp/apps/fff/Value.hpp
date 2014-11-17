#ifndef HEADER_fff_Value_hpp_ALREADY_INCLUDED
#define HEADER_fff_Value_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include <string>
#include <memory>
#include <ostream>

namespace fff { 
    class Value
    {
        public:
            enum Type {File, String, Integer};

            Value(gubg::file::File);
            Value(std::string);
            Value(long);

            bool operator<(Value) const;

            Type type() const;

            const gubg::file::File &file() const;
            const std::string &string() const;
            long integer() const;

            //Will convert a string or file, ideal for CLI args
            gubg::file::File as_file() const;

            void stream(std::ostream &) const;
            std::string to_str() const;

        private:
            struct Pimpl;
            std::shared_ptr<const Pimpl> pimpl_;
    };
    inline std::ostream &operator<<(std::ostream &os, const Value &v) {v.stream(os); return os;}
} 

#endif
