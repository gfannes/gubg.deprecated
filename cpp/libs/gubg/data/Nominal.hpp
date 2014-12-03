#ifndef HEADER_gubg_data_Nominal_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Nominal_hpp_ALREADY_INCLUDED

#include <ostream>
#include <cassert>

namespace gubg { namespace data { 

    template <size_t NrValues_>
        class Nominal
        {
            public:
                static const size_t NrValues = NrValues_;
                typedef Nominal<NrValues> Self;

                Nominal() {assert(invariants_());}
                Nominal(size_t v): v_(v) {assert(invariants_());}

                Nominal &operator=(size_t v)
                {
                    v_ = v;
                    assert(invariants_());
                    return *this;
                }

                bool operator==(const Self &rhs) const {return v_ == rhs.v_;}
                bool operator!=(const Self &rhs) const {return v_ != rhs.v_;}

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

    template <size_t NrValues_>
        std::ostream &operator<<(std::ostream &os, const Nominal<NrValues_> &nv)
        {
            return os << nv.value();
        }
} } 

#endif
