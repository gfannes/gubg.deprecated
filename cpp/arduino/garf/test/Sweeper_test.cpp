#include "garf/Sweeper.hpp"
#include "gubg/Global.hpp"

void * operator new (size_t size, void * ptr) { return ptr; }

typedef garf::Sweeper<9, 180, 360> Sweeper_;
typedef gubg::Global<Sweeper_> Sweeper;
//Sweeper sweeper; 
Sweeper_ s;

void setup()
{
    //sweeper.init();
    s.init();
}

void loop()
{
    //sweeper->process();
    s.process();
}
