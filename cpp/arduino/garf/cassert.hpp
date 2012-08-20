#ifndef garf_cassert_hpp
#define garf_cassert_hpp

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
