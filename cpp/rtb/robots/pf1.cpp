#include "rtb/Commander.hpp"
#include "gubg/file/File.hpp"
#include "gubg/macro.hpp"
#include "gubg/Plot.hpp"
#include "gubg/math/tau.hpp"
#include "gubg/bayesian/ParticleFilter.hpp"
#include <complex>
#include <deque>
#include <array>
#include <thread>

#include "gubg/distribution/Uniform.hpp"
using namespace std;
using namespace rtb;

namespace 
{
    class Commander: public rtb::Commander_crtp<Commander>
    {
        public:
            Commander():
                Commander_crtp(gubg::file::File(__FILE__).basename()),
                particleFilter_(wallDetector_, 1000){}

            //Default event handler
            template <typename Msg>
                void commander_event(Msg &msg) { log << "Lost event" << endl;}

            void commander_event(Initialize &init)
            {
                log << STREAM(init.seq) << endl;
                if (init.seq == 1)
                {
                    send(Name(name()));
                    send(Colour(color::blue, color::green));
                }
            }
            void commander_event(GameOption &option)
            {
                log << STREAM(option.id, option.value) << endl;
            }
            void commander_event(GameStarts &)
            {
                log << "START" << endl;
                const double a = 0.2;
                //send(Sweep(RobotPart::Radar, 10, -a, a));
                send(Accelerate(1));
            }
            void commander_event(Warning &warn)
            {
                log << STREAM(warn.id) << endl;
            }
            void commander_event(Radar &radar)
            {
                log << STREAM(radar.distance, (int)radar.type, radar.angle) << endl;
                if (radar.type == ObjectType::Wall)
                {
                    const Coord obs = polar(radar.distance, radar.angle);
                    particleFilter_.process(0, obs);
                    plot_.polar(particleFilter_.particles(), [](const ParticleFilter::State &s){return array<double, 3>({arg(s.coord), abs(s.coord), s.background ? 1 : 2});});
                }
                else
                {
                    particleFilter_.process(0, 0);
                }
            }
            void commander_event(Collision &collision)
            {
                log << STREAM((int)collision.type, collision.angle) << endl;
                const double a = 0.5*gubg::pi;
                send(RotateAmount(RobotPart::Robot, 2, gubg::distribution::uniform(-a, a)));
                /*
                if (collision.type == ObjectType::Wall)
                {
                    const Coord obs = polar(0.0, collision.angle);
                    particleFilter_.process(0, obs);
                    plot_.polar(particleFilter_.particles(), [](const ParticleFilter::State &s){return array<double, 3>({arg(s.coord), abs(s.coord), s.background ? 1 : 2});});
                }
                else
                {
                    particleFilter_.process(0, 0);
                }
                */
            }
            void commander_event(Info &info)
            {
                log << STREAM(info.time, info.speed, info.cannon) << endl;
                const double d = 0.1;
                const double s = 1;
                if (info.speed < s-d)
                    send(Accelerate(1));
                else if (info.speed > s+d)
                    send(Accelerate(-1));
                else
                    send(Accelerate(0));
            }
            void commander_event(Coordinates &coord)
            {
                log << STREAM(coord.x, coord.y, coord.angle) << endl;
            }

        private:
            typedef complex<double> Coord;
            struct WallDetector
            {
                struct State
                {
                    bool background;
                    Coord coord;
                    State(): background(true) {}
                };
                void updateState(State &s, int) const
                {
                    if (gubg::distribution::withProbability(0.1))
                    {
                        const double d = 5;
                        s.coord = Coord(gubg::distribution::uniform(-d, d), gubg::distribution::uniform(-d, d));
                        s.background = true;
                    }
                    else
                    {
                        const double d = 0.05;
                        s.coord += polar(gubg::distribution::uniform(0, d), gubg::distribution::uniform(0, 2*gubg::pi));
                        s.background = false;
                    }
                }
                double observation_prob(int obs, int) const {return 1.0;}
                double observation_prob(const Coord &p, const State &s) const
                {
                    const auto d = abs(p-s.coord);
                    const double lambda = 20.0;
                    return lambda*exp(-lambda*d);
                }
            };
            WallDetector wallDetector_;
            typedef gubg::bayesian::ParticleFilter<WallDetector> ParticleFilter;
            ParticleFilter particleFilter_;
            gubg::Plot plot_;
    };
}

int main()
{
    Commander commander;
    while (commander.process())
    {
    }

    return 0;
}
