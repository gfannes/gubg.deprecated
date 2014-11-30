#ifndef HEADER_gubg_data_Nominal_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Nominal_hpp_ALREADY_INCLUDED

#include <cassert>

namespace gubg { namespace data { 

    template <size_t NrValues_>
        class Nominal
        {
            public:
                static const size_t NrValues = NrValues_;

                Nominal() {assert(invariants_());}
                Nominal(size_t v): v_(v) {assert(invariants_());}

                Nominal &operator=(size_t v)
                {
                    v_ = v;
                    assert(invariants_());
                    return *this;
                }

                size_t value() const {return v_;}

            private:
                bool invariants_() const
                {
                    if (v_ >= NrValues)
                        return false;
                    return true;
                }

                size_t v_ = 0;
        };

} } 

#endif
