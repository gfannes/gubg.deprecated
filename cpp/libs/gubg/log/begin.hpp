//NO INCLUDE GUARDS HERE

#ifdef S
  #error S is already defined
#endif
#ifdef SS
  #error SS is already defined
#endif
#ifdef L_S__
  #error L_S__ is already defined
#endif
#ifdef L
  #error L is already defined
#endif
#ifdef LL
  #error LL is already defined
#endif
#ifdef L_L__
  #error L_L__ is already defined
#endif

//Make sure GUBG_MODULE or GUBG_MODULE_ is defined
#ifdef GUBG_MODULE
  #ifdef GUBG_MODULE_
    #error You should not define both GUBG_MODULE and GUBG_MODULE_
  #endif
#else
  #ifndef GUBG_MODULE_
    #error You should either define GUBG_MODULE or GUBG_MODULE_
  #endif
#endif

#ifdef GUBG_DEBUG
  #ifdef GUBG_MODULE
    #include "gubg/log/macros_normal.hpp"
  #else
    #define GUBG_MODULE GUBG_MODULE_
    #include "gubg/log/macros_debug.hpp"
  #endif
#else
  #ifdef GUBG_MODULE_
    #define GUBG_MODULE GUBG_MODULE_
    #include "gubg/log/macros_noop.hpp"
  #endif
#endif
