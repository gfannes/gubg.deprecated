#include "gubg/parse/Line.hpp"
#include "gubg/Testing.hpp"
#include <iostream>

namespace 
{
	using namespace std;

	void print(const gubg::line::Lines &lines)
	{
		cout << lines.size() << " lines" << endl;
		for (auto line: lines)
			cout << "\t" << to_s(line.end) << " (" << line.txt << ")" << endl;
	}
	bool testSplitJoin(const string &str)
	{
		return str == join(gubg::line::split(str));
	}
}

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
	TEST_TAG(main);
	{
		TEST_TAG(split);
		print(gubg::line::split(""));
		print(gubg::line::split("abc"));
		print(gubg::line::split("abc\n"));
		print(gubg::line::split("abc\r"));
		print(gubg::line::split("abc\r\n"));
		print(gubg::line::split("\n\n"));
		print(gubg::line::split("abc\ndef"));
		print(gubg::line::split("abc\r\ndef\n"));
	}
	{
		TEST_TAG(join);
		TEST_TRUE(testSplitJoin(""));
		TEST_TRUE(testSplitJoin("abc"));
		TEST_TRUE(testSplitJoin("abc\n"));
		TEST_TRUE(testSplitJoin("abc\r"));
		TEST_TRUE(testSplitJoin("abc\r\n"));
		TEST_TRUE(testSplitJoin("\n\n"));
		TEST_TRUE(testSplitJoin("abc\ndef"));
		TEST_TRUE(testSplitJoin("abc\r\ndef\n"));
	}
	{
		TEST_TAG(analyseEnds);
		using gubg::line::Line;
		Line::End end;
		TEST_TRUE(analyseEnds(end, gubg::line::split("")));         TEST_EQ(Line::None, end); 
		TEST_TRUE(analyseEnds(end, gubg::line::split("abc")));      TEST_EQ(Line::None, end); 
		TEST_TRUE(analyseEnds(end, gubg::line::split("abc\n")));    TEST_EQ(Line::Unix, end); 
		TEST_TRUE(analyseEnds(end, gubg::line::split("abc\r")));    TEST_EQ(Line::OldMac, end); 
		TEST_TRUE(analyseEnds(end, gubg::line::split("abc\r\n")));  TEST_EQ(Line::Dos, end); 
		TEST_TRUE(analyseEnds(end, gubg::line::split("\n\n")));     TEST_EQ(Line::Unix, end); 
		TEST_TRUE(analyseEnds(end, gubg::line::split("abc\ndef"))); TEST_EQ(Line::Unix, end);   

		TEST_FALSE(analyseEnds(end, gubg::line::split("abc\r\ndef\n")));
	}
	return 0;
}
#include "gubg/log/end.hpp"
