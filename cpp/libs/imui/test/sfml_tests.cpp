#include "imui/sfml.hpp"

namespace { 
    class App: public imui::App
    {
        public:
            App(): imui::App(100, 200, __FILE__) {}

        private:
    };
} 

int main()
{
    App app;
    while (app())
    {
    }
    return 0;
}
