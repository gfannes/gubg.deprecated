#ifndef HEADER_gubg_tmp_Inheritance_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_Inheritance_hpp_ALREADY_INCLUDED

#include "gubg/tmp/Types.hpp"

namespace gubg
{
    namespace tmp
    {
        //This template is based on "Modern C++ design"
        template <typename Derived, typename Base>
            class Inheritance
            {
                private:
                    //The principle is to check which Test()-method is called when passing in Derived& as argument
                    //The compiler will try its best to take the first one, e.g., considering upcasting
                    //If everything fails, the Test(...) will be chosen
                    static Small Test(const Base &);
                    static Large Test(...);
                    static Derived &createDerived();
                public:
                    enum: bool {Value = (sizeof(Test(createDerived())) == sizeof(Small))};
            };
    }
}

#endif
