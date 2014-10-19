#include "catch/catch.hpp"
#include "gubg/Strange.hpp"
using namespace gubg;

TEST_CASE("default ctor", "[strange]")
{
	Strange r;
	SECTION("default ctor should be empty")
	{
		REQUIRE(r.empty());
		REQUIRE(0 == r.size());
		REQUIRE(r.str().empty());
	}
	SECTION("copying")
	{
		Strange rr(r);
		REQUIRE(rr.empty());
		REQUIRE(0 == rr.size());
	}
	SECTION("assignment")
	{
		Strange rr;
		rr = r;
		REQUIRE(rr.empty());
		REQUIRE(0 == rr.size());
	}
	SECTION("self assignment")
	{
		r = r;
		REQUIRE(r.empty());
		REQUIRE(0 == r.size());
	}
	SECTION("clear()")
	{
		REQUIRE(r.empty());
		r.clear();
		REQUIRE(r.empty());
	}
}
TEST_CASE("creation from std::string == abc", "[strange]")
{
	std::string str("abc");

	SECTION("ctor")
	{
		Strange r(str);
		REQUIRE(!r.empty());
		REQUIRE(str.size() == r.size());
	}
	SECTION("assignment")
	{
		Strange r;
		r = str;
		REQUIRE(!r.empty());
		REQUIRE(str.size() == r.size());
	}

	Strange r(str);
	SECTION("assignment")
	{
		Strange rr;
		rr = r;
		REQUIRE(!rr.empty());
		REQUIRE(str.size() == rr.size());
	}
	SECTION("self assignment")
	{
		r = r;
		REQUIRE(!r.empty());
		REQUIRE(str.size() == r.size());
	}
	SECTION("str()")
	{
		REQUIRE(str == r.str());
	}
	SECTION("front()")
	{
		REQUIRE('a' == r.front());
	}
	SECTION("back()")
	{
		REQUIRE('c' == r.back());
	}
	SECTION("clear()")
	{
		REQUIRE(!r.empty());
		r.clear();
		REQUIRE(r.empty());
	}
	SECTION("popAll() into Strange")
	{
		Strange rr;
		REQUIRE(!r.empty());
		r.popAll(rr);
		REQUIRE(r.empty());
		REQUIRE(!rr.empty());
	}
	SECTION("popAll() into std::string")
	{
		std::string ss;
		REQUIRE(!r.empty());
		r.popAll(ss);
		REQUIRE(r.empty());
		REQUIRE(!ss.empty());
	}
	SECTION("popTo()")
	{
		Strange rr;
		SECTION("a")
		{
			REQUIRE(r.popTo(rr, 'a'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("b")
		{
			REQUIRE(r.popTo(rr, 'b'));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "bc");
		}
		SECTION("c")
		{
			REQUIRE(r.popTo(rr, 'c'));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "c");
		}
		SECTION("e")
		{
			REQUIRE(!r.popTo(rr, 'e'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("bc")
		{
			REQUIRE(r.popTo(rr, "bc"));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "bc");
		}
	}
    SECTION("diffTo() with strange")
    {
        Strange sp = r;
        SECTION("when r is not changed, sp should diff to empty")
        {
            REQUIRE(sp.diffTo(r));
            REQUIRE(sp.empty());
        }
        SECTION("when r has one char popped, sp should diff to that char")
        {
            r.popFront();
            REQUIRE(sp.diffTo(r));
            REQUIRE(sp.str() == "a");
        }
        SECTION("when r is empty, sp should not be changed: we assume r just ran out")
        {
            r.clear();
            REQUIRE(sp.diffTo(r));
            REQUIRE(sp.str() == "abc");
        }
        SECTION("when sp is empty, we expect failure")
        {
            sp.clear();
            REQUIRE(!sp.diffTo(r));
        }
        SECTION("is sp was popped, we expect a failure")
        {
            sp.popFront();
            REQUIRE(!sp.diffTo(r));
        }
        SECTION("the end of r is not checked")
        {
            r.popBack();
            REQUIRE(sp.diffTo(r));
            REQUIRE(sp.empty());
        }
        SECTION("the end of sp is not checked")
        {
            sp.popBack();
            sp.popBack();
            r.popFront();
            r.popFront();
            REQUIRE(sp.str() == "a");
            REQUIRE(sp.diffTo(r));
            REQUIRE(sp.str() == "ab");
        }
    }
	SECTION("popUntil() exclusive")
	{
		Strange rr;
		SECTION("a")
		{
			REQUIRE(r.popUntil(rr, 'a'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "bc");
		}
		SECTION("b")
		{
			REQUIRE(r.popUntil(rr, 'b'));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "c");
		}
		SECTION("c")
		{
			REQUIRE(r.popUntil(rr, 'c'));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "");
		}
		SECTION("e")
		{
			REQUIRE(!r.popUntil(rr, 'e'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("ab")
		{
			REQUIRE(r.popUntil(rr, "ab"));
			REQUIRE(rr.str() == "");
			REQUIRE(r.str() == "c");
		}
		SECTION("bc")
		{
			REQUIRE(r.popUntil(rr, "bc"));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "");
		}
		SECTION("BC")
		{
			REQUIRE(!r.popUntil(rr, "BC"));
			REQUIRE(rr.str() == "");
			REQUIRE(r.str() == "abc");
		}
	}
	SECTION("popUntil() inclusive")
	{
		Strange rr;
		SECTION("a")
		{
			REQUIRE(r.popUntil(rr, 'a', true));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "bc");
		}
		SECTION("b")
		{
			REQUIRE(r.popUntil(rr, 'b', true));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "c");
		}
		SECTION("c")
		{
			REQUIRE(r.popUntil(rr, 'c', true));
			REQUIRE(rr.str() == "abc");
			REQUIRE(r.str() == "");
		}
		SECTION("e")
		{
			REQUIRE(!r.popUntil(rr, 'e', true));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("ab")
		{
			REQUIRE(r.popUntil(rr, "ab", true));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "c");
		}
		SECTION("bc")
		{
			REQUIRE(r.popUntil(rr, "bc", true));
			REQUIRE(rr.str() == "abc");
			REQUIRE(r.str() == "");
		}
		SECTION("BC")
		{
			REQUIRE(!r.popUntil(rr, "BC", true));
			REQUIRE(rr.str() == "");
			REQUIRE(r.str() == "abc");
		}
	}
	SECTION("popCharIf")
	{
		REQUIRE(!r.popCharIf('b'));
		REQUIRE(r.popCharIf('a'));
		REQUIRE(r.popCharIf('b'));
		REQUIRE(r.popCharIf('c'));
		REQUIRE(!r.popCharIf('e'));
	}
	SECTION("popCharBackIf")
	{
		REQUIRE(!r.popCharBackIf('b'));
		REQUIRE(r.popCharBackIf('c'));
		REQUIRE(r.popCharBackIf('b'));
		REQUIRE(r.popCharBackIf('a'));
		REQUIRE(!r.popCharBackIf('e'));
	}
	SECTION("popFront")
	{
		REQUIRE(r.popFront());
        REQUIRE(r.front() == 'b');
		REQUIRE(r.popFront());
        REQUIRE(r.front() == 'c');
		REQUIRE(r.popFront());
        REQUIRE(r.empty());
		REQUIRE(!r.popFront());
	}
	SECTION("popBack")
	{
		REQUIRE(r.popBack());
        REQUIRE(r.back() == 'b');
		REQUIRE(r.popBack());
        REQUIRE(r.back() == 'a');
		REQUIRE(r.popBack());
        REQUIRE(r.empty());
		REQUIRE(!r.popBack());
	}
	SECTION("popChar")
	{
		char ch;
		REQUIRE(r.popChar(ch));
		REQUIRE('a' == ch);
		REQUIRE(r.popChar(ch));
		REQUIRE('b' == ch);
		REQUIRE(r.popChar(ch));
		REQUIRE('c' == ch);
		REQUIRE(!r.popChar(ch));
	}
	SECTION("popString")
	{
		SECTION("")
		{
			REQUIRE(r.popString(""));
			REQUIRE(r.str() == "abc");
		}
		SECTION("a")
		{
			REQUIRE(r.popString("a"));
			REQUIRE(r.str() == "bc");
		}
		SECTION("abc")
		{
			REQUIRE(r.popString("abc"));
			REQUIRE(r.str() == "");
		}
		SECTION("A")
		{
			REQUIRE(!r.popString("A"));
			REQUIRE(r.str() == "abc");
		}
	}
	SECTION("popLine")
	{
		Strange line;
		SECTION("abc")
		{
			REQUIRE(r.popLine(line));
			REQUIRE(line.str() == "abc");
			REQUIRE(r.empty());
			REQUIRE(!r.popLine(line));
		}
		SECTION("abc\n")
		{
			std::string str = "abc\n";
			r = str;
			REQUIRE(r.popLine(line));
			REQUIRE(line.str() == "abc");
			REQUIRE(r.empty());
			REQUIRE(!r.popLine(line));
		}
		SECTION("abc\ndef")
		{
			std::string str = "abc\ndef";
			r = str;
			REQUIRE(r.popLine(line));
			REQUIRE(line.str() == "abc");
			REQUIRE(!r.empty());
			REQUIRE(r.popLine(line));
			REQUIRE(line.str() == "def");
		}
	}
}
TEST_CASE("numbers", "[strange]")
{
	Strange r;
	SECTION("popDecimal()")
	{
		struct TV
		{
			std::string str;
			long nbr;
		};
		std::vector<TV> tvs = {
			{"0", 0},
			{"1", 1},
			{"42", 42},
			{"1000000", 1000000},
		};
		for (auto tv: tvs)
		{
			{
				long nbr;
				r = tv.str;
				REQUIRE(r.popDecimal(nbr));
				REQUIRE(tv.nbr == nbr);

				std::string str = "-"; str += tv.str;
				r = str;
				REQUIRE(r.popDecimal(nbr));
				REQUIRE(-tv.nbr == nbr);
			}
			{
				int nbr;
				r = tv.str;
				REQUIRE(r.popDecimal(nbr));
				REQUIRE(tv.nbr == nbr);

				std::string str = "-"; str += tv.str;
				r = str;
				REQUIRE(r.popDecimal(nbr));
				REQUIRE(-tv.nbr == nbr);
			}
		}
	}
	SECTION("popFloat()")
	{
		std::string flt = "-42.123";
		r = flt;
		double d;
		REQUIRE(r.popFloat(d));
		REQUIRE(-42.123 == d);
	}
}
TEST_CASE("subtract", "[strange]")
{
}
