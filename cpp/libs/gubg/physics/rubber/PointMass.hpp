#ifndef HEADER_gubg_physics_rubber_PointMass_hpp_ALREADY_INCLUDED
#define HEADER_gubg_physics_rubber_PointMass_hpp_ALREADY_INCLUDED

#include <array>

namespace gubg
{
    namespace rubber
    {
        template <typename T, int Dim>
            class PointMass
            {
                public:
                    T mass;
                    typedef std::array<T, Dim> Location;
                    Location location;
                    typedef std::array<T, Dim> Speed;
                    Speed speed;

                    PointMass():
                        mass(0.0){}

                private:
            };
    }
}

#endif
