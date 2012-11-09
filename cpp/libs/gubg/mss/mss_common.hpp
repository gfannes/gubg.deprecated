#ifndef HEADER_gubg_mss_mss_common_hpp_ALREADY_INCLUDED
#define HEADER_gubg_mss_mss_common_hpp_ALREADY_INCLUDED

#define MSS_RC_VAR l_mss_rc_var

#define MSS_DEFAULT_CODES_WITHOUT_OK InternalError, IllegalArgument, NotImplemented, False, NullPointer, InvalidSharedPtr, UnknownError, PimplError, LastCode = 9
#define MSS_DEFAULT_CODES OK, MSS_DEFAULT_CODES_WITHOUT_OK

namespace gubg
{
    namespace mss
    {
        //We use this for return code storage for bools and ints
        enum class ReturnCode: int {OK = 0, StartOfCodes = -128, MSS_DEFAULT_CODES_WITHOUT_OK};

        template <typename RC, RC has_ok = RC::OK>
            bool isOK(RC rc){return RC::OK == rc;}
        inline bool isOK(bool b){return b;}
    }
}

#endif
