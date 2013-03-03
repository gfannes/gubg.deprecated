#ifndef HEADER_gubg_physics_Newton_hpp_ALREADY_INCLUDED
#define HEADER_gubg_physics_Newton_hpp_ALREADY_INCLUDED

namespace gubg
{
    namespace physics
    {
        template <typename PM, typename Force, typename Duration>
            void updateSpeed(PM &pm, Force force, Duration duration)
            {
                duration /= pm.mass;
                Vector::multiply2All(force, duration);
                Vector::add(pm.speed, force);
            }
    }
}

#endif
