#include "gubg/Strange.hpp"

#define GUBG_MODULE "Strange"
#include "gubg/log/begin.hpp"
namespace gubg { 

	Strange::Strange():
        s_(0), l_(0)
    {
    }
	Strange::Strange(const std::string &str)
        :s_(str.data()), l_(str.size())
    {
    }

	bool Strange::empty() const
    {
        return l_ == 0;
    }
	size_t Strange::size() const
    {
        return l_;
    }
	std::string Strange::str() const
    {
        return std::string(s_, l_);
    }
	char Strange::front() const
    {
        assert(s_ && l_);
        return *s_;
    }
	char Strange::back() const
    {
        assert(s_ && l_);
        return s_[l_-1];
    }
	void Strange::clear()
    {
        s_ = 0;
        l_ = 0;
    }

	bool Strange::contains(char ch) const
	{
		return nullptr != std::memchr(s_, ch, l_);
	}

	void Strange::popAll(Strange &res)
	{
		assert(invariants_());
		res = *this;
		clear();
	}
	void Strange::popAll(std::string &res)
	{
		assert(invariants_());
		Strange s;
		popAll(s);
		res = s.str();
	}
	//Does not pop ch
	bool Strange::popTo(Strange &res, const char ch)
	{
		assert(invariants_());
		if (empty())
			return false;
		char *ptr = (char*)std::memchr(s_, ch, l_);
		if (!ptr)
			return false;
		res.s_ = s_;
		res.l_ = ptr-s_;
		forward_(res.l_);
		return true;
	}
	//Does not pop str
	bool Strange::popTo(Strange &res, const std::string &str)
	{
		assert(invariants_());
		if (str.empty())
			return false;
		//We will iteratively search for ch, and try to match the rest of str
		const char ch = str[0];
		const size_t s = str.size();
		const Strange sp = *this;
		while (char *ptr = (char*)std::memchr(s_, ch, l_))
		{
			const size_t l = ptr-s_;
			forward_(l);
			if (size() < s)
				break;
			if (0 == std::memcmp(str.data(), s_, s))
			{
				res.s_ = sp.s_;
				res.l_ = s_-res.s_;
				return true;
			}
		}
		*this = sp;
		return false;
	}
	bool Strange::diffTo(const Strange &strange)
	{
        if (empty())
            //Already at the end
			return false;
        if (strange.empty())
            //If strange is empty, we cannot trust its s_. We assume strange ran to its end and will return everything in res.
            return true;
		if (strange.s_ < s_)
			//Not really expected
			return false;
        l_ = (strange.s_ - s_);
        return true;
	}
	//Pops ch too, set inclusive to true if you want ch to be included in res
	bool Strange::popUntil(Strange &res, const char ch, bool inclusive)
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
	bool Strange::popUntil(std::string &res, const char ch, bool inclusive)
	{
		Strange s;
		if (!popUntil(s, ch, inclusive))
			return false;
		res = s.str();
		return true;
	}
	bool Strange::popUntil(Strange &res, const std::string &str, bool inclusive)
	{
		assert(invariants_());
		if (str.empty())
			return true;
		const size_t s = str.size();
		if (size() < s)
			//We are to small to match str
			return false;
		const auto ch = str.front();
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
	bool Strange::popUntil(std::string &res, const std::string &str, bool inclusive)
	{
		Strange s;
		if (!popUntil(s, str, inclusive))
			return false;
		res = s.str();
		return true;
	}
	bool Strange::popDecimal(long &res)
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
	bool Strange::popFloat(double &res)
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

	bool Strange::popCharIf(const char ch)
	{
		assert(invariants_());
		if (empty())
			return false;
		if (*s_ != ch)
			return false;
		forward_(1);
		return true;
	}
	bool Strange::popCharBackIf(const char ch)
	{
		assert(invariants_());
		if (empty())
			return false;
		if (s_[l_-1] != ch)
			return false;
		shrink_(1);
		return true;
	}
	bool Strange::popFront()
	{
		assert(invariants_());
		if (empty())
			return false;
		forward_(1);
		return true;
	}
	bool Strange::popBack()
	{
		assert(invariants_());
		if (empty())
			return false;
		shrink_(1);
		return true;
	}
	bool Strange::popChar(char &ch)
	{
		assert(invariants_());
		if (empty())
			return false;
		ch = *s_;
		forward_(1);
		return true;
	}

    bool Strange::popString(std::string &str, size_t nr)
    {
        assert(invariants_());
        if (l_ < nr)
            return false;
        str.assign(s_, nr);
        forward_(nr);
        return true;
    }
    bool Strange::popStringIf(const std::string &str)
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

    bool Strange::popLine(Strange &line)
    {
        S();
        assert(invariants_());

        if (empty())
            return false;

        L(str());

        //We start looking for 0xa because that is the most likely indicator of an end-of-line
        //0xd can occur on its own, but that is old-mac style, which is not used anymore
        const char *ptr = (const char *)std::memchr(s_, '\x0a', l_);
        if (!ptr)
        {
            L("No 0xa found, lets look for a 0xd (old-mac)");
            ptr = (const char *)std::memchr(s_, '\x0d', l_);
            if (!ptr)
            {
                L("old-mac wasn't found either, we return everything we got, this is the last line");
                line = *this;
                forward_(l_);
                return true;
            }
            L("An old-mac end-of-line was found");
            line.s_ = s_;
            line.l_ = ptr-s_;
            forward_(line.l_+1);
            return true;
        }
        L("0xa was found, we still need to determine if it is a unix or dos style end-of-line");
        if (ptr == s_)
        {
            L("This is an empty line, it does not make sense to check for 0xd");
            line.s_ = s_;
            line.l_ = 0;
            forward_(1);
            return true;
        }
        L("We have to check for 0xd");
        if (ptr[-1] == '\x0d')
        {
            L("This line is dos-style terminated");
            line.s_ = s_;
            line.l_ = ptr-s_-1;
            forward_(line.l_+2);
            return true;
        }
        L("No 0xd was found before ptr so we have a unix-style terminated line");
        line.s_ = s_;
        line.l_ = ptr-s_;
        forward_(line.l_+1);
        return true;
    }

    template <typename T>
        bool Strange::popLSB_(T &v)
        {
            assert(invariants_());
            if (l_ < sizeof(v))
                return false;
            v = 0;
            for (int i = 0; i < sizeof(v); ++i)
            {
                T tmp = *(const T *)(s_+i);
                tmp <<= i*8;
                v |= tmp;
            }
            forward_(sizeof(v));
            return true;
        }
    bool Strange::popLSB(std::uint8_t &v)  { return popLSB_(v); }
    bool Strange::popLSB(std::uint16_t &v) { return popLSB_(v); }
    bool Strange::popLSB(std::uint32_t &v) { return popLSB_(v); }
    bool Strange::popLSB(std::uint64_t &v) { return popLSB_(v); }
    bool Strange::popLSB(std::int8_t &v)   { return popLSB_(v); }
    bool Strange::popLSB(std::int16_t &v)  { return popLSB_(v); }
    bool Strange::popLSB(std::int32_t &v)  { return popLSB_(v); }
    bool Strange::popLSB(std::int64_t &v)  { return popLSB_(v); }

    template <typename T>
        bool Strange::popMSB_(T &v)
        {
            assert(invariants_());
            if (l_ < sizeof(v))
                return false;
            v = 0;
            for (int i = 0; i < sizeof(v); ++i)
            {
                v <<= 8;
                v |= *(std::uint8_t*)(s_+i);
            }
            forward_(sizeof(v));
            return true;
        }
    bool Strange::popMSB(std::uint8_t &v)  { return popMSB_(v); }
    bool Strange::popMSB(std::uint16_t &v) { return popMSB_(v); }
    bool Strange::popMSB(std::uint32_t &v) { return popMSB_(v); }
    bool Strange::popMSB(std::uint64_t &v) { return popMSB_(v); }
    bool Strange::popMSB(std::int8_t &v)   { return popMSB_(v); }
    bool Strange::popMSB(std::int16_t &v)  { return popMSB_(v); }
    bool Strange::popMSB(std::int32_t &v)  { return popMSB_(v); }
    bool Strange::popMSB(std::int64_t &v)  { return popMSB_(v); }

    bool Strange::popCount(size_t nr)
    {
        if (l_ < nr)
            return false;
        forward_(nr);
        return true;
    }
    bool Strange::popCount(Strange &res, size_t nr)
    {
        if (l_ < nr)
            return false;
        res.s_ = s_;
        res.l_ = nr;
        forward_(nr);
        return true;
    }

    bool Strange::popRaw(char *dst, size_t nr)
    {
        if (!dst)
            return false;
        if (l_ < nr)
            return false;
        std::memcpy(dst, s_, nr);
        forward_(nr);
        return true;
    }

    void Strange::ltrim(const char ch)
    {
        while (!empty() && ch == front())
            popFront();
    }

    //Privates
    bool Strange::invariants_() const
    {
        if (!s_ && l_)
            return false;
        return true;
    }
    void Strange::forward_(const size_t nr)
    {
        assert(nr <= l_);
        l_ -= nr;
        s_ += nr;
    }
    void Strange::shrink_(const size_t nr)
    {
        assert(nr <= l_);
        l_ -= nr;
    }

} 
#include "gubg/log/end.hpp"
