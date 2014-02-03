#ifndef HEADER_gubg_trace_Msg_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_Msg_hpp_ALREADY_INCLUDED

#include <string>
#include <sstream>

namespace gubg { namespace trace { 
    class Msg
    {
        public:
            Msg(size_t category): category_(category) { }

            size_t category() const {return category_;}
            std::string str() const {return oss_.str();}

            template <typename T>
                Msg &operator<<(const T &v) {oss_ << v; return *this;}

        private:
            const size_t category_;
            std::ostringstream oss_;
    };
} } 

#endif
