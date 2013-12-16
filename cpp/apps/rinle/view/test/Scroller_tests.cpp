#include "rinle/view/Scroller.hpp"
#include "gubg/pattern/SignalSlot.hpp"
#include "nana/gui/widgets/form.hpp"
#include "nana/gui/wvl.hpp"
#include <thread>
#include <chrono>
#include <string>

class DataSender
{
	public:
		DataSender(): thread_(std::ref(*this)) {}
		~DataSender()
		{
			quit_ = true;
			thread_.join();
		}

		void operator()()
		{
			int i = 0;
			while (!quit_)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				std::ostringstream oss;
				switch (i%4)
				{
					case 0: break;
					case 1: oss << "ERROR::"; break;
					case 2: oss << "WARNING::"; break;
					case 3: oss << "OK::"; break;
				}
				oss << "This is iteration " << i;
				signal.emit(oss.str());
				++i;
			}
		}
		gubg::pattern::Signal<std::string> signal;

	private:
		volatile bool quit_ = false;
		std::thread thread_;
};
int main()
{
	DataSender dataSender;

	nana::gui::form window(nana::rectangle(0, 0, 500, 300));
	window.caption(STR("Scroller tests"));

	rinle::view::Scroller scroller(window, nana::rectangle(0, 200, 500, 100), 6);
	scroller.connect(dataSender.signal);

    window.show();

	nana::gui::exec();

	return 0;
}
