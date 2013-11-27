#ifndef HEADER_gubg_Macro_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Macro_hpp_ALREADY_INCLUDED

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
#define GUBG_GET_6TH_ARG(_1,_2,_3,_4,_5,_6,...) _6
#define GUBG_GET_7TH_ARG(_1,_2,_3,_4,_5,_6,_7,...) _7
#define GUBG_GET_8TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,...) _8
#define GUBG_GET_9TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,_9,...) _9
#define GUBG_GET_10TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,...) _10
#define GUBG_GET_11TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,...) _11

#if 0
//Found on http://stackoverflow.com/questions/2308243/macro-returning-the-number-of-arguments-it-is-given-in-c
#define GUBG_NARG2(...) (sizeof((int[]){0, ##__VA_ARGS__})/sizeof(int)-1)

//Found on http://jacquesmattheij.com/C+Preprocessor+Hell
//Does not work (for gcc at least)
#define GUBG_REVERSE 5, 4, 3, 2, 1, 0
#define GUBG_ARGN(_1, _2, _3, _4, _5, N, ...) N
#define GUBG_NARG_(dummy, ...) GUBG_ARGN(__VA_ARGS__)
#define GUBG_NARG(...) GUBG_NARG_(dummy, __VA_ARGS__, GUBG_REVERSE)

#define GUBG_CONCAT(a, b)   GUBG_CONCAT_1(a, b)
#define GUBG_CONCAT_1(a, b) GUBG_CONCAT_2(a, b)
#define GUBG_CONCAT_2(a, b) a##b

#define FIELD_0(...)
#define FIELD_1(field, ...) somestruct.field=field;
#define FIELD_2(field, ...) somestruct.field=field; FIELD_1(__VA_ARGS__)
#define FIELD_3(field, ...) somestruct.field=field; FIELD_2(__VA_ARGS__)
#define FIELD_4(field, ...) somestruct.field=field; FIELD_3(__VA_ARGS__)
#define FIELD_5(field, ...) somestruct.field=field; FIELD_4(__VA_ARGS__)
#define FIELDS_(N, ...) GUBG_CONCAT(FIELD_, N)(__VA_ARGS__)
#define FIELDS(...) FIELDS_(GUBG_NARG(__VA_ARGS__), ##__VA_ARGS__)
#endif

//Streaming of variables
#define STREAM_FORMAT_A(var) "{" #var ": " << var <<
#define STREAM_FORMAT_B(var) ", " #var ": " << var <<
#define STREAM_1(_1) STREAM_FORMAT_A(_1)
#define STREAM_2(_1,_2                         )  STREAM_1(_1                        ) STREAM_FORMAT_B(_2)
#define STREAM_3(_1,_2,_3                      )  STREAM_2(_1,_2                     ) STREAM_FORMAT_B(_3)
#define STREAM_4(_1,_2,_3,_4                   )  STREAM_3(_1,_2,_3                  ) STREAM_FORMAT_B(_4)
#define STREAM_5(_1,_2,_3,_4,_5                )  STREAM_4(_1,_2,_3,_4               ) STREAM_FORMAT_B(_5)
#define STREAM_6(_1,_2,_3,_4,_5,_6             )  STREAM_5(_1,_2,_3,_4,_5            ) STREAM_FORMAT_B(_6)
#define STREAM_7(_1,_2,_3,_4,_5,_6,_7          )  STREAM_6(_1,_2,_3,_4,_5,_6         ) STREAM_FORMAT_B(_7)
#define STREAM_8(_1,_2,_3,_4,_5,_6,_7,_8       )  STREAM_7(_1,_2,_3,_4,_5,_6,_7      ) STREAM_FORMAT_B(_8)
#define STREAM_9(_1,_2,_3,_4,_5,_6,_7,_8,_9    )  STREAM_8(_1,_2,_3,_4,_5,_6,_7,_8   ) STREAM_FORMAT_B(_9)
#define STREAM_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) STREAM_9(_1,_2,_3,_4,_5,_6,_7,_8,_9) STREAM_FORMAT_B(_10)
#define STREAM_MACRO_CHOOSER(...) GUBG_GET_11TH_ARG(__VA_ARGS__, STREAM_10,STREAM_9,STREAM_8,STREAM_7,STREAM_6,STREAM_5,STREAM_4,STREAM_3,STREAM_2,STREAM_1)
#define STREAM(...) STREAM_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) "}"

#ifdef RANGE
#error RANGE is already defined
#endif
#define RANGE(ctr) ctr.begin(), ctr.end()

#endif
