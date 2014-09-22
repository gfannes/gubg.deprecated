#ifndef HEADER_gubg_dynlib_Visibility_hpp_ALREADY_INCLUDED
#define HEADER_gubg_dynlib_Visibility_hpp_ALREADY_INCLUDED

#if defined _WIN32 || defined __CYGWIN__

#define GUBG_DYNLIB_IMPORT __declspec(dllimport)
#define GUBG_DYNLIB_EXPORT __declspec(dllexport)
#define GUBG_DYNLIB_LOCAL  

#else

#define GUBG_DYNLIB_IMPORT __attribute__ ((visibility ("default")))
#define GUBG_DYNLIB_EXPORT __attribute__ ((visibility ("default")))
#define GUBG_DYNLIB_LOCAL  __attribute__ ((visibility ("hidden")))

#endif

#endif
