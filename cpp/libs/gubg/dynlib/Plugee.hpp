#ifndef HEADER_gubg_dynlib_Plugee_hpp_ALREADY_INCLUDED
#define HEADER_gubg_dynlib_Plugee_hpp_ALREADY_INCLUDED

#include "gubg/dynlib/Library.hpp"

namespace gubg { namespace dynlib { 

    class Plugee
    {
        public:
            Plugee(const file::File fn): fn_(fn) {}

            template <typename T>
                T *create()
                {
                    loadLib_();
                    typedef void*(*F_ptr)(void);
                    F_ptr f_ptr = (F_ptr)lib_.get("create");
                    if (!f_ptr)
                        return 0;
                    return (T*)f_ptr();
                }

        private:
            void loadLib_()
            {
                if (lib_.valid())
                    return;
                lib_ = Library(fn_);
            }
            const file::File fn_;
            Library lib_;
    };

} } 

#endif
