#include "gubg/tty/Endpoint.hpp"
#include "gubg/l.hpp"

namespace 
{
    class Arduino: public gubg::tty::Endpoint_crtp<Arduino>
    {
        public:
            Arduino():
                i(0),
                Endpoint_crtp("/dev/ttyACM0"){}
            void endpoint_received(int ch)
            {
                std::cout << (char)ch;
            }
        private:
            int i;
    };

}

int main()
{
    Arduino a;
    int i = 0;
    for (;;)
    {
        a.process();
    }
    return 0;
}
