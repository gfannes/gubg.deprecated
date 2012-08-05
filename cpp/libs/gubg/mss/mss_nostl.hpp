#ifndef gubg_mss_mss_nostl_hpp
#define gubg_mss_mss_nostl_hpp

#ifdef gubg_mss_mss_hpp
#error You cannot mix normal with nostl mss
#endif
#include "gubg/mss/mss_common.hpp"
#include "gubg/macro.hpp"

namespace gubg
{
    namespace mss
    {
        //ReturnCode wrappers for enum, bool and void
        template <typename T>
            struct ReturnCodeWrapper
            {
                typedef T ReturnCodeT;

                T v_;

                ReturnCodeWrapper():
                    v_(T::OK){}

                T get() const { return v_; }

                bool set(T v)
                {
                    if (isOK(v))
                    {
                        v_ = T::OK;
                        return true;
                    }
                    v_ = v;
                    return false;
                }
                template <typename OT>
                    bool set(OT ot, T v = T::UnknownError) { return set(OT::OK == ot ? T::OK : v); }
                template <typename P>
                    bool set(P *p, T v = T::NullPointer) { return set(0 != p ? T::OK : v); }
                bool set(bool b, T v = T::False) { return set(b ? T::OK : v); }
            };
        template <>
            struct ReturnCodeWrapper<bool>
            {
                typedef bool ReturnCodeT;

                ::gubg::mss::ReturnCode v_;

                ReturnCodeWrapper():
                    v_(ReturnCode::OK){}

                bool get() const {return ReturnCode::OK == v_;}

                bool set(ReturnCode v)
                {
                    if (isOK(v))
                    {
                        v_ = ReturnCode::OK;
                        return true;
                    }
                    v_ = v;
                    return false;
                }
                template <typename OT>
                    bool set(OT ot, ReturnCode v = ReturnCode::UnknownError) { return set(OT::OK == ot ? ReturnCode::OK : v); }
                template <typename P>
                    bool set(P *p, ReturnCode v = ReturnCode::NullPointer) { return set(0 != p ? ReturnCode::OK : v); }
                bool set(bool b, ReturnCode v = ReturnCode::False) { return set(b ? ReturnCode::OK : v); }
            };
        template <>
            struct ReturnCodeWrapper<void>
            {
                typedef void ReturnCodeT;

                void get(){}

                template <typename OT>
                    bool set(OT v) { return OT::OK == v; }
                bool set(bool b) { return b; }
                template <typename P>
                    bool set(P *p)
                    {
                        if (!p)
                            return false;
                        return true;
                    }
            };
    }
}
#define MSS_BEGIN(type) \
    typedef ::gubg::mss::ReturnCodeWrapper<type> mss_return_code_wrapper_type; \
    typedef mss_return_code_wrapper_type::ReturnCodeT mss_return_code_type; \
    mss_return_code_wrapper_type MSS_RC_VAR

#define MSS_END() \
    return MSS_RC_VAR.get()

#define MSS_DIRECT(v) \
    do { \
        if (!MSS_RC_VAR.set(v)) \
            MSS_END(); \
    } while (false)

#define MSS_TRANS(v, nc) \
    do { \
        if (!MSS_RC_VAR.set(v, mss_return_code_type::nc)) \
            MSS_END(); \
    } while (false)

#define MSS_1(v)          MSS_DIRECT(v)
#define MSS_2(v, nc)      MSS_TRANS(v, nc)
#define MSS_MACRO_CHOOSER(...) GUBG_GET_3TH_ARG(__VA_ARGS__,MSS_2,MSS_1)
#define MSS(...) MSS_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define MSS_L(c) MSS_DIRECT(mss_return_code_type::c)

#endif
