#ifndef gubg_threading_Suicider_hpp
#define gubg_threading_Suicider_hpp

//Make sure you put this RAII at the beginning of the thread function

#define GUBG_SUICIDE_AT_EXIT() gubg::Suicider<decltype(this)> l_gubg_suicider_raii(this)

namespace gubg
{
    template <typename This>
        class Suicider
        {
            public:
                Suicider(This thiz):
                    this_(thiz){}
                ~Suicider()
                {
                    delete this_;
                }
            private:
                This this_;
        };
}

#endif
