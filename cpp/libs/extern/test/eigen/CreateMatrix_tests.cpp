#include "catch/catch.hpp"
#include "Eigen/Eigen"
#include <iostream>
using Eigen::MatrixXd;
using namespace std;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
TEST_CASE("Create a matrix", "[]")
{
    S();
    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    L(endl << m);
}
#include "gubg/log/end.hpp"
