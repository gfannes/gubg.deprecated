#ifndef gubg_macro_hpp
#define gubg_macro_hpp

//Macro dispatching
//Example usage:
//
//#define <MACRO_NAME>_1(_1) bla(_1)
//#define <MACRO_NAME>_2(_1, _2) bli(_1, _2)
//#define <MACRO_NAME>_MACRO_CHOOSER(...) GUBG_GET_3TH_ARG(__VA_ARGS__, <MACRO_NAME>_2,<MACRO_NAME>_1)
//#define <MACRO_NAME>(...) <MACRO_NAME>_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define GUBG_GET_2TH_ARG(_1,_2,...) _2
#define GUBG_GET_3TH_ARG(_1,_2,_3,...) _3
#define GUBG_GET_4TH_ARG(_1,_2,_3,_4,...) _4
#define GUBG_GET_5TH_ARG(_1,_2,_3,_4,_5,...) _5
#define GUBG_GET_11TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,...) _11

#endif
