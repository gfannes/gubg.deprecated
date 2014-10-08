#ifndef HEADER_gubg_Strange_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Strange_hpp_ALREADY_INCLUDED

#include "gubg/parse/numbers/Integer.hpp"
#include <cstdlib>
#include <cassert>
#include <ostream>
#include <cstring>

//TODO: the methods that take a Strange& as argument are currently not correct when this argument
//is the same as the this pointer

namespace gubg
{

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

            void popAll(Strange &res);
            void popAll(std::string &res);

            //Does not pop ch
            bool popTo(Strange &res, const char ch);
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

            bool popString(const std::string &str);

            bool popLine(Strange &line);

        private:
            bool invariants_() const;
            void forward_(const size_t nr);
            void shrink_(const size_t nr);

            const char *s_;
            size_t l_;
    };

}

namespace std
{
    inline ostream &operator<<(ostream &os, const gubg::Strange &strange)
    {
        os << strange.str();
        return os;
    }
}

#endif
