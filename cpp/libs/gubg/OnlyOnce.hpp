#ifndef HEADER_gubg_OnlyOnce_hpp_ALREADY_INCLUDED
#define HEADER_gubg_OnlyOnce_hpp_ALREADY_INCLUDED

namespace gubg
{
    struct OnlyOnce
    {
        OnlyOnce():
            firstTime_(true){}
        OnlyOnce(bool firstTime):
            firstTime_(firstTime){}

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
