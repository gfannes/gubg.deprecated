#include "gubg/math/random/DefaultEngine.hpp"

namespace gubg { namespace math { namespace random { 

        std::mt19937 defaultEngine(std::random_device().operator()());

} } } 
