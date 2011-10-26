#ifndef gubg_mss_hpp
#define gubg_mss_hpp

//Main success scenario: enables easy working with return codes
//A general scope (e.g., a function body) looks like this:
//
//MSS_BEGIN(ReturnCodeT);
// => Linear sequence of statements that are checked for being part of the main success scenario
//    As soon as one fails, we jump to MSS_FAIL()
//    If nothing fails, we jump to MSS_END() => when the complete mss is ok, we don't execute whatever is between MSS_FAIL() and MSS_END()
//MSS(some_function_that_returns_ReturnCodeT());
//MSS_T(some_function_that_returns_WhateverT(), code_upon_failure); => rc is set to code_upon_failure when some_function_that_returns_WhateverT() fails
//...
// => You can return or continue here
//
//MSS_FAIL();
// => rc holds the specific failure (or translated failure)
//
//MSS_END(); or MSS_RETURN();
// => You can return or continue here

#include "gubg/mss/info.hpp"

namespace gubg
{
    namespace mss
    {
        template <typename T>
            inline bool isOK(const T &t) {return T::OK == t;}
        template <>
            inline bool isOK<bool>(const bool &b) {return b;}
        template <typename T>
            inline bool isOK(T *p) {return 0 != p;}
    }
}

#define MSS_BEGIN(type)      typedef type gubg_return_code_type; \
                             gubg_return_code_type rc = gubg_return_code_type::OK

#define MSS_FAIL()           goto gubg_mss_end_label; \
                             gubg_mss_fail_label:

#define MSS_END()            gubg_mss_end_label:

#define MSS_RETURN()         gubg_mss_end_label: \
                             return rc

#define MSS_FAIL_OR_RETURN() gubg_mss_fail_label: \
                             gubg_mss_end_label: \
                             return rc

#define L_MSS_LOG(l, c, msg) \
{ \
    auto info = gubg::mss::getInfo(c); \
    auto l_level = (gubg::mss::Level::Unknown == gubg::mss::Level::l ? info.level : gubg::mss::Level::l); \
    std::cout << l_level << "::" << info.type << "::" << info.code << " " << msg << std::endl; \
    if (gubg::mss::Level::Fatal == l_level) \
    { \
        std::cout << "\tFATAL ERROR ENCOUNTERED, GAME OVER..." << std::endl; \
        exit(-1); \
    } \
}

#define MSS_(level, v, msg) \
    do { \
        if (!gubg::mss::isOK(rc = (v))) \
        { \
            L_MSS_LOG(level, rc, msg); \
            goto gubg_mss_fail_label; \
        } \
    } while (false)
#define MSS(v) MSS_(Unknown, v, "")
#define MSS_L_(level, c, msg) MSS_(level, return_code_type::c, msg)
#define MSS_L(c) MSS_L_(Unknown, return_code_type::c, "")
#define MSS_T_(level, v, nv, msg) \
    do { \
        if (!gubg::mss::isOK(v)) \
        { \
            rc = gubg_return_code_type::nv; \
            L_MSS_LOG(level, rc, msg); \
            goto gubg_mss_fail_label; \
        } \
    } while (false)
#define MSS_T(v, nv) MSS_T_(Unknown, v, nv, "")

#define MSS_DEFAULT_CODES OK, InternalError, IllegalArgument, NotImplemented
#define MSS_CODE_BEGIN(type) \
    namespace { \
        typedef type l_gubg_mss_type; \
        const char *l_gubg_mss_type_as_string = #type; \
        MSS_CODE_(OK, OK); \
        MSS_CODE_(Fatal, NotImplemented); \
        MSS_CODE_(Critical, InternalError); \
        MSS_CODE_(Error, IllegalArgument);
#define MSS_CODE_(level, code) gubg::mss::InfoSetter<l_gubg_mss_type> l_gubg_mss_InfoSetter_ ## _ ## code(l_gubg_mss_type::code, gubg::mss::Level::level, l_gubg_mss_type_as_string, #code)
#define MSS_CODE(code) MSS_CODE_(Error, code)
#define MSS_CODE_END() }

//#define L_GUBG_MSS_USE_EXCEPTIONS
#ifdef L_GUBG_MSS_USE_EXCEPTIONS
//This achieves more or less the same as the goto-based macros above, but performance is much much worse
#define MSS_BEGIN(type) typedef type gubg_return_code_type; type rc = type::OK; try {
#define MSS_FAIL() } catch (gubg_return_code_type &) { }
#define MSS_END()
#define MSS_RETURN() return rc

#define MSS(v) do { if (!gubg::mss::isOK(rc = (v))) {throw rc;} } while (false)
#define MSS_T(v, nv) do { if (!gubg::mss::isOK(v)) {rc = gubg_return_code_type::nv; throw rc;} } while (false)
#endif

#endif
