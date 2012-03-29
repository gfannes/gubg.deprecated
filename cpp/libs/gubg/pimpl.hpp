#ifndef gubg_pimpl_hpp
#define gubg_pimpl_hpp

//PIMPL macro helpers
//See mss/test/pimpl_test.cpp for usage

//Cunning trick, we assume that the caller has written "struct", which we hijack
#define PIMPL \
        Pimpl; \
        shared_ptr<Pimpl> pimpl_; \
    public: \
        enum pimpl_tag{OK}; \
        bool pimplOK() const {return (bool)pimpl_;} \
    private: \
        struct Pimpl

#define PIMPL_FORWARD_MSS(name) \
    template <typename ...Args> \
    ReturnCode name(Args... args) const \
    { \
        if (!pimpl_) \
            return ReturnCode::PimplError; \
        return pimpl_->name(args...); \
    }

#define PIMPL_FORWARD_RAW(rc, name) \
    template <typename ...Args> \
    rc name(Args... args) const \
    { \
        if (!pimpl_) \
            throw string("PimplError when forwarding to \"" #rc " " #name "(...) const\""); \
        return pimpl_->name(args...); \
    }

#endif
