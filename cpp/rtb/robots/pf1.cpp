#include "rtb/Commander.hpp"
#include "gubg/file/File.hpp"
#include "gubg/macro.hpp"
using namespace std;
using namespace rtb;

namespace 
{
    class Commander: public rtb::Commander_crtp<Commander>
    {
        public:
            Commander():
                Commander_crtp(gubg::file::File(__FILE__).basename()){}

            //Default event handler
            template <typename Msg>
                void commander_event(Msg &msg) { log << "Lost event" << std::endl;}

            void commander_event(Initialize &init)
            {
                log << STREAM(init.seq) << endl;
            }
            void commander_event(GameOption &option)
            {
                log << STREAM(option.id, option.value) << endl;
            }
            void commander_event(GameStarts &)
            {
                log << "START" << endl;
            }
            void commander_event(Warning &warn)
            {
                log << STREAM(warn.id) << endl;
            }
            void commander_event(Radar &radar)
            {
                log << STREAM(radar.distance, (int)radar.type, radar.angle) << endl;
            }
            void commander_event(Coordinates &coord)
            {
                log << STREAM(coord.x, coord.y, coord.angle) << endl;
            }

        private:
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
