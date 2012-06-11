//Compares matrix*vector between uBLAS and Eigen => Eigen is the clear winner

#include "gubg/mss.hpp"
#include "gubg/chrono/Stopwatch.hpp"

#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"
using namespace boost::numeric::ublas;

#include "Eigen/Dense"
using Eigen::MatrixXd;
using Eigen::VectorXd;

#include <thread>
#include <iostream>
#define L(m) std::cout<<m<<std::endl

namespace 
{
    const double stay = 0.7;
    const size_t dim = 100;
}
struct ub
{
    typedef matrix<double> Matrix;
    typedef vector<double> Vector;
    template <typename M, typename V>
        static void prod_(M &m, V &v)
        {
            v = prod(m, v);
        }
};

struct ei
{
    typedef MatrixXd Matrix;
    typedef VectorXd Vector;
    template <typename M, typename V>
        static void prod_(M &m, V &v)
        {
            v = m * v;
        }
};

//A basic test that simulates a simple circular Markov chain
template <typename T>
struct Test
{
    typedef typename T::Matrix Matrix;
    typedef typename T::Vector Vector;
    Matrix m;
    Vector v;
    Test(size_t dim):
        m(dim, dim), v(dim)
    {
        for (size_t i = 0; i < dim; ++i)
        {
            m(i,i) = stay;
            m(i, i == 0 ? dim-1 : i-1) = 1.0-stay;
        }
        v(0) = 1.0;
    }
    size_t operator()()
    {
        gubg::chrono::Stopwatch<> sw;
        size_t nr = 0;
        while (sw.mark().total_elapse() < std::chrono::milliseconds(500))
        {
            ++nr;
            T::prod_(m, v);
        }
        return nr;
    }
};

int main()
{
    MSS_BEGIN(int);

    for (size_t dim = 1; dim < 100; ++dim)
    {
        size_t ubc = Test<ub>(dim)();
        size_t eic = Test<ei>(dim)();
        L("dim: " << dim << ", ubc: " << ubc << ", eic: " << eic << ", frac: " << (double)eic/ubc);
    }

    MSS_END();
}
