#include "garf/iostream.hpp"
#include "gubg/Macro.hpp"
#include "Arduino.h"
using namespace garf;

int i = 123;
int j = 234;

void setup()
{
}

void loop()
{
    cout << millis() << endl;
    cout << '\t' << "i:  " << i << " " << &i << endl;
    cout << '\t' << "j:  " << j << " " << &j << endl;
    cout << '\t' << STREAM(i, j) << endl;
    delay(1000);
}
