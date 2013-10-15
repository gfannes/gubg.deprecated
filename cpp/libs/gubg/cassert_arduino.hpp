#ifndef HEADER_gubg_cassert_arduino_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cassert_arduino_hpp_ALREADY_INCLUDED

#ifndef ARDUINO
#error This is only meant for arduino
#endif

#ifdef assert
#error assert already defined
#endif

#ifdef NDEBUG
#define assert(condition) \
    do {} while (false)
#else
#define assert(condition) \
    do {if (!condition) {asm volatile("jmp 0");}} while (false)
 
#endif

#endif
