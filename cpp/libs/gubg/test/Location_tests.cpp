#include "gubg/Location.hpp"
#include "iostream"

#define LOG() std::cout << GUBG_HERE() << std::endl

int main()
{
    std::cout << GUBG_HERE() << std::endl;
    LOG();
    return 0;
}
