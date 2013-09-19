#ifndef HEADER_rtb_Messages_hpp_ALREADY_INCLUDED
#define HEADER_rtb_Messages_hpp_ALREADY_INCLUDED

namespace rtb
{
    enum class ObjectType {Unknown = -1, Robot, Shot, Wall, Cookie, Mine};

    struct Initialize
    {
        int seq;
        Initialize(): seq(0) {}
    };
    struct GameOption
    {
        int id;
        double value;
        GameOption(): id(0), value(0) {}
    };
    struct Warning
    {
        int id;
        Warning(): id(0) {}
    };
    struct GameStarts
    {
    };
    struct Radar
    {
        double distance;
        ObjectType type;
        double angle;
        Radar(): distance(0), type(ObjectType::Unknown), angle(0) {}
    };
    struct Coordinates
    {
        double x;
        double y;
        double angle;
        Coordinates(): x(0), y(0), angle(0) {}
    };
}

#endif
