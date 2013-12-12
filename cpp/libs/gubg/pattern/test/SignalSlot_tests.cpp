#include "gubg/pattern/SignalSlot.hpp"
#include "gubg/Testing.hpp"
#include <string>
#include <sstream>

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
typedef const std::string & Msg;

class Model
{
    public:
        void setData(int i)
        {
            S();
            std::ostringstream oss; oss << i;
            data_ = oss.str();
            signal.emit(data_);
        }
        gubg::pattern::Signal<Msg> signal;

    private:
        std::string data_;
};

class View
{
    public:
        View(): slot(*this) {}

        void process(Msg msg)
        {
            S();L(STREAM(this, msg));
        }
        gubg::pattern::Slot<View, Msg> slot;
};

int main()
{
    TEST_TAG(main);

    Model model;
    View view1;
    model.signal.connect(view1.slot);
    {
        View view2;
        model.signal.connect(view2.slot);
        model.setData(42);
    }
    model.setData(42);

    return 0;
}
#include "gubg/log/end.hpp"
