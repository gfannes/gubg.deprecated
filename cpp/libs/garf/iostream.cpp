#include "garf/iostream.hpp"
#include "Arduino.h"

namespace garf { 

    ostream &endl(ostream &os)
    {
        return os << "\r\n";
    }

    ostream &ostream::operator<<(const char *cstr)
    {
        initialize_();
        Serial.print(cstr);
        return *this;
    }
    ostream &ostream::operator<<(char ch)
    {
        initialize_();
        Serial.print(ch);
        return *this;
    }
    ostream &ostream::operator<<(int i)
    {
        initialize_();
        Serial.print(i);
        return *this;
    }
    ostream &ostream::operator<<(unsigned long i)
    {
        initialize_();
        Serial.print(i);
        return *this;
    }
    ostream &ostream::operator<<(IOManip_ptr ptr)
    {
        initialize_();
        return ptr(*this);
    }
    ostream &ostream::operator<<(void * const ptr)
    {
        initialize_();
        Serial.print((unsigned int)ptr, HEX);
        return *this;
    }

    void ostream::initialize_()
    {
        if (!doInit_())
            return;
        Serial.begin(9600);
    }

    //cout global variable
    ostream cout;

} 
