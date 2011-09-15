#ifndef cubeDimension_h
#define cubeDimension_h

#include <vector>

using namespace std;

namespace gubg
{
    namespace Cube
    {
        // A generalized vector - matrix - tensor - stuff, vectors of arbitrary depth
        // Dimension< vector<vector<int> > >::dimension == 2
        // Dimension< int, 2 >::type == vector<vector<int> >
        template<typename T,int dim = -1>
        struct Dimension
        {
            typedef Dimension<T,dim-1> typemin;
            typedef vector< typename typemin::type > type;
        };
        template<typename T>
        struct Dimension<T,0>
        {
            typedef T type;
        };
        template<typename T>
        struct Dimension<T,-1>
        {
            static const int dimension = 0;
            typedef T basetype;
        };
        template<typename T>
        struct Dimension<vector<T>,-1 >
        {
            static const int dimension = Dimension<T,-1>::dimension + 1;
            typedef typename Dimension<T,-1>::basetype basetype;
        };
    }
}

#endif
