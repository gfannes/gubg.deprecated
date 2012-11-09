#ifndef HEADER_gubg_function_Linear_hpp_ALREADY_INCLUDED
#define HEADER_gubg_function_Linear_hpp_ALREADY_INCLUDED

namespace gubg
{
    namespace function
    {
        template <typename T>
            class Linear
            {
                public:
                    Linear(T x1, T y1, T x2, T y2)
                    {
                        if (x1 == x2)
                            throw std::string("Both input values are the same");
                        a_ =_ (y2-y1)/(x2-x1);
                        b_ = y1 - a_*x1;
                    }

                    T operator()(T x) const {return a_*x+b_;}

                private:
                    const T a_;
                    const T b_;
            };
    }
}

#endif
