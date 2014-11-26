#ifndef HEADER_gubg_Macro_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Macro_hpp_ALREADY_INCLUDED

//Macro dispatching
//#define <MACRO_NAME>_1(_1) bla(_1)
//#define <MACRO_NAME>_2(_1, _2) bli(_1, _2)
//#define <MACRO_NAME>(...) GUBG_GET_ARG_3((__VA_ARGS__, <MACRO_NAME>_2,<MACRO_NAME>_1))(__VA_ARGS__)
#define GUBG_GET_ARG_1_(N,...)                                 N
#define GUBG_GET_ARG_1(tuple)                                  GUBG_GET_ARG_1_ tuple
#define GUBG_GET_ARG_2_(_1,N,...)                              N
#define GUBG_GET_ARG_2(tuple)                                  GUBG_GET_ARG_2_ tuple
#define GUBG_GET_ARG_3_(_1,_2,N,...)                           N
#define GUBG_GET_ARG_3(tuple)                                  GUBG_GET_ARG_3_ tuple
#define GUBG_GET_ARG_4_(_1,_2,_3,N,...)                        N
#define GUBG_GET_ARG_4(tuple)                                  GUBG_GET_ARG_4_ tuple
#define GUBG_GET_ARG_5_(_1,_2,_3,_4,N,...)                     N
#define GUBG_GET_ARG_5(tuple)                                  GUBG_GET_ARG_5_ tuple
#define GUBG_GET_ARG_6_(_1,_2,_3,_4,_5,N,...)                  N
#define GUBG_GET_ARG_6(tuple)                                  GUBG_GET_ARG_6_ tuple
#define GUBG_GET_ARG_7_(_1,_2,_3,_4,_5,_6,N,...)               N
#define GUBG_GET_ARG_7(tuple)                                  GUBG_GET_ARG_7_ tuple
#define GUBG_GET_ARG_8_(_1,_2,_3,_4,_5,_6,_7,N,...)            N
#define GUBG_GET_ARG_8(tuple)                                  GUBG_GET_ARG_8_ tuple
#define GUBG_GET_ARG_9_(_1,_2,_3,_4,_5,_6,_7,_8,N,...)         N
#define GUBG_GET_ARG_9(tuple)                                  GUBG_GET_ARG_9_ tuple
#define GUBG_GET_ARG_10_(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...)     N
#define GUBG_GET_ARG_10(tuple)                                 GUBG_GET_ARG_10_ tuple
#define GUBG_GET_ARG_11_(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N
#define GUBG_GET_ARG_11(tuple)                                 GUBG_GET_ARG_11_ tuple

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
#define STREAM(...) GUBG_GET_ARG_11((__VA_ARGS__, STREAM_10,STREAM_9,STREAM_8,STREAM_7,STREAM_6,STREAM_5,STREAM_4,STREAM_3,STREAM_2,STREAM_1))(__VA_ARGS__) "}"

#ifdef RANGE
#error RANGE is already defined
#endif
#define RANGE(ctr) ctr.begin(), ctr.end()

#endif
