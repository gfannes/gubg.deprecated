#ifndef HEADER_gubg_mss_mss_hpp_ALREADY_INCLUDED
#define HEADER_gubg_mss_mss_hpp_ALREADY_INCLUDED

#ifdef gubg_mss_mss_nostl_hpp
#error You cannot mix normal with nostl mss
#endif

//Main success scenario: enables easy working with return codes
//A general scope (e.g., a function body) looks like this:
//
//MSS_BEGIN(ReturnCodeT);
// => Initializes a return value "MSS_RC_VAR" of type ReturnCodeT to OK
// => Linear sequence of statements that are checked for being part of the main success scenario
//    As soon as one fails, we return from the function with the failing code
//    If nothing fails, MSS_END() will return from the function with a "OK" value
//MSS(some_function_that_returns_ReturnCodeT());
//MSS(some_function_that_returns_WhateverT(), code_upon_failure); => MSS_RC_VAR is set to code_upon_failure when some_function_that_returns_WhateverT() fails
//...
// => You can return or continue here
//
//MSS_END();
// => Ends the MSS block and returns from the function

#include "gubg/mss/mss_common.hpp"
#include "gubg/mss/info.hpp"
#include "gubg/clock/timer.hpp"
#include "gubg/Location.hpp"
#include "gubg/macro.hpp"
#include <memory>
#include <set>
#include <iostream>

template <typename T>
struct AllowOtherCodes
{
    void setAllowed(T v){allowedCodes_.insert(v);}
    bool isAllowed(T v) const {return allowedCodes_.count(v);}
    std::set<T> allowedCodes_;
};

namespace gubg
{
    namespace mss
    {
        using namespace std;
        template <typename T>
            struct mss_shared_ptr { typedef std::shared_ptr<T> type; };

        template <typename T>
            struct NoOtherCodesAllowed
            {
                bool isAllowed(T) const {return false;}
            };
        template <typename T, template <typename TT> class AllowedCodesPolicy = NoOtherCodesAllowed>
            struct ReturnCodeWrapper: AllowedCodesPolicy<T>
        {
            typedef T ReturnCodeT;
            typedef AllowedCodesPolicy<T> AllowedCodesPolicyT;
            ReturnCodeWrapper():v_(T::OK){}
            T get() { return v_; }
            template <typename OT>
                bool isOK(OT ov) const { return OT::OK == ov; }
            bool isOK(T v) const {return T::OK == v || AllowedCodesPolicyT::isAllowed(v);}
            bool set(T v)
            {
                if (isOK(v))
                {
                    //Every allowed code becomes OK
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
            template <typename X>
                bool set(std::shared_ptr<X> p, T v = T::InvalidSharedPtr){return set((bool)p ? T::OK : v);}
            bool set(bool b, T v = T::False) { return set(b ? T::OK : v); }
            Level level() const {return getInfo<T>(v_).level;}
            std::string toString() const
            {
                std::ostringstream oss;
                auto info = getInfo(v_);
                oss << info.type << ":" << info.code;
                return oss.str();
            }
            T v_;
        };
#define L_CASE_CODE(code) case ReturnCode::code: return #code
        inline const char *l_MSSCode_AsString(ReturnCode code)
        {
            switch (code)
            {
                L_CASE_CODE(OK             );
                L_CASE_CODE(False          );
                L_CASE_CODE(NullPointer    );
                L_CASE_CODE(InvalidSharedPtr);
                L_CASE_CODE(InternalError  );
                L_CASE_CODE(IllegalArgument);
                L_CASE_CODE(NotImplemented );
                L_CASE_CODE(UnknownError   );
                L_CASE_CODE(PimplError   );
            }
            return "Please extend l_MSSCode_AsString in gubg/mss/mss.hpp";
        }
        template <typename T>
            const char *l_MSSCode_AsString(std::shared_ptr<T> p) {return ((bool)p ? "shared_ptr OK" : "shared_ptr INVALID");}
        template <>
            struct ReturnCodeWrapper<bool, NoOtherCodesAllowed>
            {
                typedef bool ReturnCodeT;
                ReturnCodeWrapper():v_(ReturnCode::OK){}
                bool get(){return ReturnCode::OK == v_;}
                bool isOK(bool b) const {return b;}
                bool isOK(ReturnCode v) const {return ReturnCode::OK == v;}
                bool set(ReturnCode v)
                {
                    if (isOK(v))
                    {
                        //Every allowed code becomes OK
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
                template <typename X>
                    bool set(std::shared_ptr<X> p, ReturnCode v = ReturnCode::InvalidSharedPtr){return set((bool)p ? ReturnCode::OK : v);}
                bool set(bool b, ReturnCode v = ReturnCode::False) { return set(b ? ReturnCode::OK : v); }
                Level level() const {return getInfo<ReturnCode>(v_).level;}
                std::string toString() const
                {
                    return l_MSSCode_AsString(v_);
                }
                ReturnCode v_;
            };
        //When using directly with int, 0 indicates OK
        template <>
            struct ReturnCodeWrapper<int, NoOtherCodesAllowed>
            {
                typedef ReturnCode ReturnCodeT;
                ReturnCodeWrapper():v_(int(ReturnCode::OK)){}
                int get()
                {
                    return v_;
                }
                bool isOK(bool b) const {return b;}
                bool isOK(int v) const {return int(ReturnCode::OK) == v;}
                bool isOK(ReturnCode v) const {return ReturnCode::OK == v;}
                bool set(ReturnCode v)
                {
                    if (isOK(v))
                    {
                        //Every allowed code becomes OK
                        v_ = int(ReturnCode::OK);
                        return true;
                    }
                    v_ = int(v);
                    return false;
                }
                template <typename OT, OT has_ok = OT::OK>
                    bool set(OT ot, ReturnCode v = ReturnCode::UnknownError) { return set(OT::OK == ot ? ReturnCode::OK : v); }
                template <typename OT, typename OT::pimpl_tag is_pimpl = OT::pimpl_tag::OK>
                    bool set(OT ot, ReturnCode v = ReturnCode::PimplError) { return set(ot.pimplOK() ? ReturnCode::OK : v); }
                template <typename P>
                    bool set(P *p, ReturnCode v = ReturnCode::NullPointer) { return set(0 != p ? ReturnCode::OK : v); }
                template <typename X>
                    bool set(std::shared_ptr<X> p, ReturnCode v = ReturnCode::InvalidSharedPtr){return set((bool)p ? ReturnCode::OK : v);}
                bool set(bool b, ReturnCode v = ReturnCode::False) { return set(b ? ReturnCode::OK : v); }
                Level level() const {return getInfo<ReturnCode>((ReturnCode)(v_)).level;}
                std::string toString() const
                {
                    return l_MSSCode_AsString((ReturnCode)(v_));
                }
                int v_;
            };
        template <>
            struct ReturnCodeWrapper<void, NoOtherCodesAllowed>
            {
                typedef void ReturnCodeT;
                ReturnCodeWrapper():l_(Level::OK){}
                void get(){}
                template <typename OT>
                    bool set(OT v)
                    {
                        l_ = getInfo<OT>(v).level;
                        return OT::OK == v;
                    }
                bool set(bool b)
                {
                    v_ = (b ? "true" : "false");
                    l_ = (b ? Level::OK : Level::Error);
                    return b;
                }
                template <typename P>
                    bool set(P *p)
                    {
                        if (!p)
                        {
                            v_ = "null pointer";
                            l_ = Level::Error;
                            return false;
                        }
                        v_ = "non-null pointer";
                        l_ = Level::OK;
                        return true;
                    }
                template <typename OT>
                    bool isOK(OT ov) const {return OT::OK == ov;}
                Level level() const {return l_;}
                std::string toString() const {return v_;}
                std::string v_;
                Level l_;
            };
        //std::shared_ptr support
        template <typename T>
            struct ReturnCodeWrapper<std::shared_ptr<T>, NoOtherCodesAllowed>
            {
                typedef std::shared_ptr<T> ReturnCodeT;
                ReturnCodeWrapper(){}
                ReturnCodeT get(){return v_;}
                bool isOK(bool b) const {return b;}
                bool isOK(ReturnCode v) const {return ReturnCode::OK == v;}
                bool isOK(ReturnCodeT v) const {return (bool)v;}
                bool set(ReturnCode v)
                {
                    if (isOK(v))
                    {
                        //Every allowed code becomes OK
                        return true;
                    }
                    v_.reset();
                    return false;
                }
                template <typename OT>
                    bool set(OT ot, ReturnCode v = ReturnCode::UnknownError) { return set(OT::OK == ot ? ReturnCode::OK : v); }
                template <typename P>
                    bool set(P *p, ReturnCode v = ReturnCode::NullPointer) { return set(0 != p ? ReturnCode::OK : v); }
                template <typename X>
                    bool set(std::shared_ptr<X> p, ReturnCode v = ReturnCode::InvalidSharedPtr){return set((bool)p ? ReturnCode::OK : v);}
                bool set(bool b, ReturnCode v = ReturnCode::False) { return set(b ? ReturnCode::OK : v); }

		//Used in MSS_SET_PTR()
		void set_ptr(ReturnCodeT p) { v_ = p; }
		void set_ptr(T *p) { v_.reset(p); }

		Level level() const {return gubg::mss::Level::Error;}
		std::string toString() const
		{
			return l_MSSCode_AsString(v_);
		}
		ReturnCodeT v_;
            };
        template <typename ReturnCodeWrapper>
            struct SuccessChecker
            {
                public:
                    SuccessChecker(const gubg::Location &location, ReturnCodeWrapper &rcw):
                        success_(false),
                        location_(location),
                        returnCodeWrapper_(rcw){}
                    ~SuccessChecker()
                    {
                        if (!success_)
                            std::cout << "MSS UNSUCCESSFUL::" << location_ << "::" << msg_ << "::" << returnCodeWrapper_.toString() << std::endl;
                    }
                    void setMessage(const std::string &msg){msg_ = msg;}
                    void indicateSuccess(){success_ = true;}
                private:
                    bool success_;
                    std::string msg_;
                    std::string returnCodeInfo_;
                    gubg::Location location_;
                    ReturnCodeWrapper &returnCodeWrapper_;
            };

        //A helper template to fix a bug in decltype (decltype can currently not be combined with a scope operator)
        template <typename TT> struct l_declfix {typedef TT T;};

        template <typename X>
            bool isOK(std::shared_ptr<X> p){return (bool)p;}
        template <typename X, typename X::pimpl_tag is_pimpl = X::pimpl_tag::OK>
            bool isOK(X x){return x.pimplOK();}

        struct ElapseReporter
        {
            ElapseReporter(const gubg::Location &location, const std::string &msg):
                location_(location), msg_(msg), timer_(ResetType::NoAuto){}
            ~ElapseReporter()
            {
                double elapse = timer_.difference();
                std::cout << location_ << "::" << msg_ << " => " << elapse << " sec" << endl;
            }
            private:
            const gubg::Location location_;
            const string msg_;
            gubg::Timer timer_;
        };
    }
}

#define MSS_BEGIN_RC_WRAPPER(...)    typedef gubg::mss::ReturnCodeWrapper<__VA_ARGS__> mss_return_code_wrapper_type; \
                                     typedef mss_return_code_wrapper_type::ReturnCodeT mss_return_code_type; \
                                     mss_return_code_wrapper_type MSS_RC_VAR
#define MSS_BEGIN_1(type)            MSS_BEGIN_RC_WRAPPER(type); \
                                     S()
#define MSS_BEGIN_2(type, msg)       MSS_BEGIN_RC_WRAPPER(type); \
                                     S(); L(msg)
#define MSS_BEGIN_MACRO_CHOOSER(...) GUBG_GET_3TH_ARG(__VA_ARGS__, MSS_BEGIN_2,MSS_BEGIN_1)
#define MSS_BEGIN(...)               MSS_BEGIN_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define MSS_BEGIN_ALLOW(type)        MSS_BEGIN_RC_WRAPPER(type, AllowOtherCodes); \
                                     S(type)

#define MSS_BEGIN_J()        gubg::mss::ReturnCodeWrapper<void> MSS_RC_VAR;
#define MSS_BEGIN_PROFILE(t, msg) std::ostringstream l_mss_elapse_reporter_msg; \
    l_mss_elapse_reporter_msg << msg; \
gubg::mss::ElapseReporter l_mss_elapse_reporter(GUBG_HERE(), l_mss_elapse_reporter_msg.str()); \
MSS_BEGIN(t)

#define MSS_ALLOW(v)         MSS_RC_VAR.setAllowed(mss_return_code_type::v)

#define MSS_END()            return MSS_RC_VAR.get()
#define MSS_FAIL()           gubg_mss_fail_label:

//Normally, MSS_RC_VAR.get() is OK
#define MSS_RETURN_OK()      return MSS_RC_VAR.get()

//Checks if the main success scenario is successfully completed. It detects direct return statements and exceptions
#define MSS_BEGIN_(type, msg) \
    MSS_BEGIN(type); \
{ \
    gubg::mss::SuccessChecker<mss_return_code_wrapper_type> l_mss_success_checker(GUBG_HERE(), MSS_RC_VAR); \
    { \
        std::ostringstream l_mss_success_checker_m; l_mss_success_checker_m << msg; \
        l_mss_success_checker.setMessage(l_mss_success_checker_m.str()); \
    }

#define MSS_END_() \
    l_mss_success_checker.indicateSuccess(); \
    MSS_END(); \
}

//Logging
#define L_MSS_LOG_PRIM(rc_str, level, msg)  L_(GUBG_HERE() << " " << level << "::" << rc_str << msg)
#define L_MSS_LOG_PRIM_(rc_str, level, msg) std::cout << GUBG_HERE() << " " << level << "::" << rc_str << msg << std::endl
#define L_MSS_LOG(l, rc, msg) \
{ \
    auto level = (gubg::mss::Level::Unknown == gubg::mss::Level::l ? rc.level() : gubg::mss::Level::l); \
    L_MSS_LOG_PRIM(rc.toString(), level, msg); \
}

//Will cause a "controlled crash" if v is not OK or false
#define MSS_ENSURE(v, msg) if (!gubg::mss::isOK(v)) \
{ \
    L_MSS_LOG_PRIM_("<" #v ">", gubg::mss::Level::Fatal, "MSS_ENSURE(" << #v << ") failure, termination is inevitable: " << msg); \
    typedef void(*Crash)(); \
    Crash crash = 0; \
    crash(); \
}

//Direct handling, v should be of the same type as specified in MSS_BEGIN(type)
#define MSS_DIRECT(level, v, msg) \
    do { \
        if (!MSS_RC_VAR.set(v)) \
        { \
            L_MSS_LOG(level, MSS_RC_VAR, msg); \
            MSS_END(); \
        } \
    } while (false)
#define MSS_DIRECT_Q(v) \
    do { \
        if (!MSS_RC_VAR.set(v)) \
        { \
            MSS_END(); \
        } \
    } while (false)

//Transformation of _any_ type v in a hardcoded value nc. nc should be a value of the enum type specified in MSS_BEGIN(type)
#define MSS_TRANS(level, v, nc, msg) \
    do { \
        if (!MSS_RC_VAR.set(v, mss_return_code_type::nc)) \
        { \
            L_MSS_LOG(level, MSS_RC_VAR, msg); \
            MSS_END(); \
        } \
    } while (false)
#define MSS_TRANS_Q(v, nc) \
    do { \
        if (!MSS_RC_VAR.set(v, mss_return_code_type::nc)) \
        { \
            MSS_END(); \
        } \
    } while (false)

#define MSS_1(v)          MSS_DIRECT(Unknown, v, "")
#define MSS_2(v, nc)      MSS_TRANS(Unknown, v, nc, "")
#define MSS_3(v, nc, msg) MSS_TRANS(Unknown, v, nc, msg)
#define MSS_MACRO_CHOOSER(...) GUBG_GET_4TH_ARG(__VA_ARGS__, MSS_3,MSS_2,MSS_1)
#define MSS(...) MSS_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define MSS_Q_1(v)          MSS_DIRECT_Q(v)
#define MSS_Q_2(v, nc)      MSS_TRANS_Q(v, nc)
#define MSS_Q_MACRO_CHOOSER(...) GUBG_GET_3TH_ARG(__VA_ARGS__, MSS_Q_2,MSS_Q_1)
#define MSS_Q(...) MSS_Q_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define MSS__1(level) You have to specify also a value
#define MSS__2(level, v)          MSS_DIRECT(level, v, "")
#define MSS__3(level, v, nc)      MSS_TRANS(level, v, nc, "")
#define MSS__4(level, v, nc, msg) MSS_TRANS(level, v, nc, msg)
#define MSS__MACRO_CHOOSER(...) GUBG_GET_5TH_ARG(__VA_ARGS__, MSS__4,MSS__3,MSS__2,MSS__1)
#define MSS_(...) MSS__MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

//Direct return of a hardcoded value, c should be a value of the enum type specified in MSS_BEGIN(type)
#define MSS_L_(level, c, msg) MSS_DIRECT(level, mss_return_code_type::c, msg)
#define MSS_L(c) MSS_L_(Unknown, c, "")

#define MSS_QL(c) MSS_DIRECT_Q(mss_return_code_type::c)

#define MSS_IS_OK(v) gubg::mss::isOK(v)

//Support for setting the internal shared_ptr (when MSS_BEGIN(shared_ptr<>) is used)
#define MSS_SET_PTR(ptr) \
	do { \
		MSS(ptr); \
		MSS_RC_VAR.set_ptr(ptr); \
	} while (false)
#define MSS_SET_PTR_Q(ptr) \
	do { \
		MSS_Q(ptr); \
		MSS_RC_VAR.set_ptr(ptr); \
	} while (false)

//Allows you to test for one specific failure value, which is typically something like "NotFound"
//If v == c, the block under this macro will be skipped
#define MSS_SKIP_IF(v, c) \
    for (auto l_v = v, l_firstTime = gubg::mss::l_declfix<decltype(l_v)>::T::OK; l_firstTime == gubg::mss::l_declfix<decltype(l_v)>::T::OK; l_firstTime = gubg::mss::l_declfix<decltype(l_v)>::T::False) \
if (l_v == gubg::mss::l_declfix<decltype(l_v)>::T::c) {} \
else if (!MSS_RC_VAR.isOK(l_v)) {MSS(l_v);} \
else

//Allows you to test for one specific failure value, which is typically an error value that can be rectified, allowing MSS to continue
//If v == c, the block under this macro will be entered
#define MSS_DO_IF(v, c) \
    for (mss_return_code_type l_v = v, l_firstTime = mss_return_code_type::OK; l_firstTime == mss_return_code_type::OK; l_firstTime = mss_return_code_type::False) \
if (l_v != mss_return_code_type::c) \
{ \
    if (!MSS_RC_VAR.isOK(l_v)) {MSS(l_v);} \
} \
else 

//Allows you to handle a failure locally. If you need to know what went wrong, use a switch statement instead
#define MSS_IF_FAIL(v) \
    if (!MSS_RC_VAR.isOK(v))

//Allows integration with functions of incompatible return types, this is goto-based
#define MSS_J_(level, v, msg) \
    do { \
        if (!MSS_RC_VAR.set(v)) \
        { \
            L_MSS_LOG(level, MSS_RC_VAR, msg); \
            goto gubg_mss_fail_label; \
        } \
    } while (false)
#define MSS_J(v) MSS_J_(Unknown, v, "")

#define MSS_CODES_BEGIN(type) \
    namespace { \
        typedef type l_mss_type; \
        const char *l_mss_type_as_string = #type; \
        MSS_CODE_(OK, OK); \
        MSS_CODE_(Fatal, NotImplemented); \
        MSS_CODE_(Critical, InternalError); \
        MSS_CODE_(Error, IllegalArgument); \
        MSS_CODE_(Error, False); \
        MSS_CODE_(Error, NullPointer); \
        MSS_CODE_(Error, InvalidSharedPtr); \
        MSS_CODE_(Error, PimplError); \
        MSS_CODE_(Error, UnknownError);
#define MSS_CODE_(level, code) gubg::mss::InfoSetter<l_mss_type> l_mss_InfoSetter_ ## _ ## code(l_mss_type::code, gubg::mss::Level::level, l_mss_type_as_string, #code)
#define MSS_CODE(code) MSS_CODE_(Error, code)
#define MSS_CODES_END() }

#endif
