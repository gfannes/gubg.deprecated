#ifndef gubg_l_hpp
#define gubg_l_hpp

#ifdef L
#error L is already defined...
#endif

#include <iostream>
#define L(m) std::cout << m << std::endl

#endif
