#ifndef aoeuaoeu
#define aoeuaoeu

//Stream functionality meant to be used with Message

namespace gubg { namespace log {

    struct Cout
    {
        std::ostream &os = std::cout;
    };
    template <typename Name>
        class File
        {
            public:
                std::ostream &os;
                File(): os(getOrCreate_()) { }
            private:
                static std::unique_ptr<std::ofstream> fos_;
                static std::ostream &getOrCreate_()
                {
                    if (!fos_)
                        fos_.reset(new std::ofstream(Name::filename(), std::ios_base::out | std::ios_base::trunc));
                    return *fos_;
                }
        };
    template <typename Name>
        std::unique_ptr<std::ofstream> File<Name>::fos_;

    namespace helper
    {
        //A mutex with static life time, locking follows the lifetime of an instance
        template <typename Stream>
            struct TypeLock
            {
                TypeLock() {mutex_.lock();}
                ~TypeLock() {mutex_.unlock();}
                static std::mutex mutex_;
            };
        template <typename Stream>
            std::mutex TypeLock<Stream>::mutex_;
    }
    //We place the TypeLock first to make sure the stream is locked before it is created
    //the first time. The Stream is accessible as-is
    template <typename Stream>
        class LockedStream: helper::TypeLock<Stream>, public Stream { };

} }

#endif
