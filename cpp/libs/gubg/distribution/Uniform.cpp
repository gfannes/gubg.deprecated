#include "gubg/distribution/Uniform.hpp"

namespace gubg
{
    namespace distribution
    {
        std::mt19937 uniform(std::random_device().operator()());
    }
}
