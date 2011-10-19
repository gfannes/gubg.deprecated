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

#define MSS(v)       do { if (!gubg::mss::isOK(rc = (v)))                      {                                goto gubg_mss_fail_label;} } while (false)
#define MSS_L(c)     do { if (!gubg::mss::isOK(rc = gubg_return_code_type::c)) {                                goto gubg_mss_fail_label;} } while (false)
#define MSS_T(v, nv) do { if (!gubg::mss::isOK(v))                             {rc = gubg_return_code_type::nv; goto gubg_mss_fail_label;} } while (false)


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
