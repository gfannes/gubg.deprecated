#include "gubg/pattern/Observer.hpp"
#include "gubg/Testing.hpp"
#include <string>

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
class Producer: public gubg::pattern::Observable<std::string>
{
	public:
	private:
};
struct Consumer
{
	bool dirty = false;
	Consumer(): observer(*this) {}

	void notify(std::string msg)
	{
		S();L("Received " << msg);
		dirty = true;
	}
	gubg::pattern::Observer_ftop<Consumer, std::string> observer;
};

int main()
{
	TEST_TAG(main);
	Producer prod;
	Consumer cons1, cons2;
	TEST_TRUE(prod.addObserver(cons1.observer));
	//We cannot add more than one observable to an observer
	TEST_FALSE(prod.addObserver(cons1.observer));
	TEST_TRUE(prod.addObserver(cons2.observer));

	TEST_FALSE(cons1.dirty);
	TEST_FALSE(cons2.dirty);
	prod.notifyObservers("test123");
	TEST_TRUE(cons1.dirty);
	TEST_TRUE(cons2.dirty);
	return 0;
}
#include "gubg/log/end.hpp"
