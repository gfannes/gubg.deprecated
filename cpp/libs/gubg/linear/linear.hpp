#ifndef HEADER_gubg_linear_linear_hpp_ALREADY_INCLUDED
#define HEADER_gubg_linear_linear_hpp_ALREADY_INCLUDED

#include <vector>

// This class implements a linear transformation of the type
// x=Ax'+t, where x are the coordinates wrt old coordinates and x' are those wrt new coordinates
template<typename T>
class LinearTransform
{
public:
    
    bool new2Old(T &old, const T &new);
    bool old2New(T &new, const T &old);
private:
    vector<T> mA;
    T mT;
};

#endif
