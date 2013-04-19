#include "gubg/distribution/Engine.hpp"

namespace gubg
{
    namespace distribution
    {
        std::mt19937 engine(std::random_device().operator()());
    }
}
