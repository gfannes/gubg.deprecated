#ifndef gubg_OnlyOnce_hpp
#define gubg_OnlyOnce_hpp

namespace gubg
{
    struct OnlyOnce
    {
        OnlyOnce():
            firstTime_(true){}

        bool operator()()
        {
            bool ft = firstTime_;
            firstTime_ = false;
            return ft;
        }

        void reset(){firstTime_ = true;}

        private:
            bool firstTime_;
    };
}

#endif
