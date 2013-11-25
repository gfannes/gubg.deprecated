#include "gubg/Testing.hpp"
#include <thread>

#define GUBG_MODULE_ "Thread"
#include "gubg/log/begin.hpp"
struct Thread
{
	Thread(): thr_(std::ref(*this)) {S();}
	~Thread() {S();thr_.join();}
	void operator()()
	{
		S();
	}
	std::thread thr_;
};
#include "gubg/log/end.hpp"

#define GUBG_MODULE_ "ThreadSD"
#include "gubg/log/begin.hpp"
struct ThreadSD
{
	ThreadSD(): thr_(std::ref(*this)) {S();}
	~ThreadSD() {S();thr_.detach();}
	void operator()()
	{
		S();
		delete this;
	}
	std::thread thr_;
};
#include "gubg/log/end.hpp"

#define GUBG_MODULE_ "main"
#include "gubg/log/begin.hpp"
int main()
{
	TEST_TAG(main);
	{
		Thread thr;
	}
	{
		new ThreadSD;
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
	return 0;
}
#include "gubg/log/end.hpp"
