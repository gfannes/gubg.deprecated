#ifndef HEADER_gubg_Strange_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Strange_hpp_ALREADY_INCLUDED

#include "gubg/parse/numbers/Integer.hpp"
#include <cstdlib>
#include <cassert>
#include <ostream>
#include <cstring>

namespace gubg
{
    class Strange
    {
        public:
            Strange():s_(0), l_(0){}
            Strange(const std::string &str):s_(str.data()), l_(str.size()){}

            bool empty() const {return l_ == 0;}
            size_t size() const {return l_;}
            std::string str() const {return std::string(s_, l_);}
            char front() const { assert(s_ && l_); return *s_;}
            void clear() {s_ = 0; l_ = 0;}

            void popAll(Strange &res)
            {
                assert(invariants_());
                res = *this;
                clear();
            }
            void popAll(std::string &res)
            {
                assert(invariants_());
                Strange s;
                popAll(s);
                res = s.str();
            }
            //Does not pop ch
            bool popTo(Strange &res, const char ch)
            {
                assert(invariants_());
                if (empty())
                    return false;
                for (size_t i = 0; i < l_; ++i)
                    if (s_[i] == ch)
                    {
                        res.s_ = s_;
                        res.l_ = i;
                        forward_(i);
                        return true;
                    }

                return false;
            }
            //Pops ch too, set inclusive to true if you want ch to be included in res
            bool popUntil(Strange &res, const char ch, bool inclusive = false)
            {
                assert(invariants_());
                if (empty())
                    return false;
                for (size_t i = 0; i < l_; ++i)
                    if (s_[i] == ch)
                    {
                        res.s_ = s_;
                        res.l_ = i + (inclusive ? 1 : 0);
                        forward_(i+1);
                        return true;
                    }

                return false;
            }
            bool popUntil(std::string &res, const char ch, bool inclusive = false)
            {
                Strange s;
                if (!popUntil(s, ch, inclusive))
                    return false;
                res = s.str();
                return true;
            }
            bool popUntil(Strange &res, const std::string &str, bool inclusive = false)
            {
                assert(invariants_());
                if (str.empty())
                    return true;
                const auto ch = str.front();
                const size_t s = str.size();
                const auto l2check = l_-s+1;
                for (size_t i = 0; i < l2check; ++i)
                    if (s_[i] == ch)
                    {
                        //Pontential match, check the rest of str
                        if (!std::memcmp(str.data(), s_+i, s))
                        {
                            res.s_ = s_;
                            res.l_ = i + (inclusive ? s : 0);
                            forward_(i+s);
                            return true;
                        }
                    }
                return false;
            }
            bool popUntil(std::string &res, const std::string &str, bool inclusive = false)
            {
                Strange s;
                if (!popUntil(s, str, inclusive))
                    return false;
                res = s.str();
                return true;
            }
#if 0
            template <typename Int>
                bool popDecimal(Int &i)
                {
                    assert(invariants_());
                    if (empty())
                        return false;
                    const size_t BufLen = 32;
                    char buffer[BufLen+1];
                    const size_t nr = std::min(BufLen, l_);
                    std::copy(s_, s_+nr, buffer);
                    buffer[nr] = '\0';
                    char *e;
                    long l = std::strtol(buffer, &e, 10);
                    if (!e)
                        return false;
                    i = l;
                    forward_(e-buffer);
                    return true;
                }
#else
            bool popDecimal(long &res)
            {
                assert(invariants_());
                if (empty())
                    return false;
                size_t l = l_;
                if (!parse::numbers::read(res, s_, l))
                    return false;
                forward_(l);
                return true;
            }
            template <typename Int>
                bool popDecimal(Int &i)
                {
                    long l;
                    if (!popDecimal(l))
                        return false;
                    i = l;
                    return true;
                }
#endif
            bool popFloat(double &res)
            {
                assert(invariants_());
                if (empty())
                    return false;
                char *e = 0;
                double d = std::strtod(s_, &e);
                if (e == s_)
                    return false;
                res = d;
                forward_(e-s_);
                return true;
            }

            bool popCharIf(const char ch)
            {
                assert(invariants_());
                if (empty())
                    return false;
                if (*s_ != ch)
                    return false;
                forward_(1);
                return true;
            }
            bool popCharBack(const char ch)
            {
                assert(invariants_());
                if (empty())
                    return false;
                if (s_[l_-1] != ch)
                    return false;
                shrink_(1);
                return true;
            }
            bool popBack()
            {
                assert(invariants_());
                if (empty())
                    return false;
                shrink_(1);
                return true;
            }
            bool popChar(char &ch)
            {
                assert(invariants_());
                if (empty())
                    return false;
                ch = *s_;
                forward_(1);
                return true;
            }

            bool popString(const std::string &str)
            {
                assert(invariants_());
                const auto s = str.size();
                if (l_ < s)
                    return false;
                if (std::memcmp(str.data(), s_, s))
                    return false;
                forward_(s);
                return true;
            }


        private:
            bool invariants_() const
            {
                if (!s_ && l_)
                    return false;
                return true;
            }
            void forward_(const size_t nr)
            {
                assert(nr <= l_);
                l_ -= nr;
                s_ += nr;
            }
            void shrink_(const size_t nr)
            {
                assert(nr <= l_);
                l_ -= nr;
            }
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
