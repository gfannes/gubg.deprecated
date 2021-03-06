#ifndef HEADER_gubg_Strange_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Strange_hpp_ALREADY_INCLUDED

#include "gubg/parse/numbers/Integer.hpp"
#include <cstdlib>
#include <cassert>
#include <ostream>
#include <cstring>
#include <cstdint>

//TODO: the methods that take a Strange& as argument are currently not correct when this argument
//is the same as the this pointer

namespace gubg {

    class Strange
    {
        public:
            Strange();
            //Make sure str outlives Strange
            Strange(const std::string &);

            bool empty() const;
            size_t size() const;
            std::string str() const;
            char front() const;
            char back() const;
            void clear();

            bool contains(char ch) const;

            bool strip(char ch);

            //Return true if !res.empty()
            bool popAll(Strange &res);
            bool popAll(std::string &res);

            //Does not pop ch or str
            bool popTo(Strange &res, const char ch);
            bool popTo(Strange &res, const std::string &str);
            //Pops ch too, set inclusive to true if you want ch to be included in res
            bool popUntil(Strange &res, const char ch, bool inclusive = false);
            bool popUntil(std::string &res, const char ch, bool inclusive = false);
            bool popUntil(Strange &res, const std::string &str, bool inclusive = false);
            bool popUntil(std::string &res, const std::string &str, bool inclusive = false);

            //this and strange are assumed to be related and have the same end
            bool diffTo(const Strange &strange);

            bool popDecimal(long &res);
            template <typename Int>
                bool popDecimal(Int &i)
                {
                    long l;
                    if (!popDecimal(l))
                        return false;
                    i = l;
                    return true;
                }
            bool popFloat(double &res);

            bool popCharIf(const char ch);
            bool popCharBackIf(const char ch);
            bool popFront();
            bool popBack();
            bool popChar(char &ch);

            bool popString(std::string &, size_t nr);
            bool popStringIf(const std::string &);

            bool popLine(Strange &line);

            void ltrim(const char ch);

            bool popLSB(std::uint8_t &);
            bool popLSB(std::uint16_t &);
            bool popLSB(std::uint32_t &);
            bool popLSB(std::uint64_t &);
            bool popLSB(std::int8_t &);
            bool popLSB(std::int16_t &);
            bool popLSB(std::int32_t &);
            bool popLSB(std::int64_t &);

            bool popMSB(std::uint8_t &);
            bool popMSB(std::uint16_t &);
            bool popMSB(std::uint32_t &);
            bool popMSB(std::uint64_t &);
            bool popMSB(std::int8_t &);
            bool popMSB(std::int16_t &);
            bool popMSB(std::int32_t &);
            bool popMSB(std::int64_t &);

            bool popCount(size_t nr);
            bool popCount(Strange &, size_t nr);

            bool popRaw(char *dst, size_t nr);

        private:
            template <typename T>
                bool popLSB_(T &);
            template <typename T>
                bool popMSB_(T &);
            bool invariants_() const;
            void forward_(const size_t nr);
            void shrink_(const size_t nr);

            const char *s_;
            size_t l_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Strange &strange)
    {
        os << strange.str();
        return os;
    }
}

#endif
