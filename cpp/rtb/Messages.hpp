#ifndef HEADER_rtb_Messages_hpp_ALREADY_INCLUDED
#define HEADER_rtb_Messages_hpp_ALREADY_INCLUDED

#include <string>

namespace rtb
{
    enum class ObjectType {Unknown = -1, Robot, Shot, Wall, Cookie, Mine};
    enum class RobotPart {Robot = 1, Cannon = 2, Radar = 4};
    namespace color
    {
        typedef const char * Type;
        Type red   = "ff0000";
        Type green = "00ff00";
        Type blue  = "0000ff";
    }

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
    struct Collision
    {
        ObjectType type;
        double angle;
        Collision(): type(ObjectType::Unknown), angle(0) {}
    };
    struct Info
    {
        double time;
        double speed;
        double cannon;
        Info(): time(0), speed(0), cannon(0) {}
    };
    struct Coordinates
    {
        double x;
        double y;
        double angle;
        Coordinates(): x(0), y(0), angle(0) {}
    };

    struct Name
    {
        std::string name;
        Name(std::string n): name(std::move(n)) {}
    };
    struct Colour
    {
        color::Type home; 
        color::Type away; 
        Colour(color::Type h, color::Type a): home(h), away(a) {}
    };
    struct Rotate
    {
        RobotPart part;
        double speed;
        Rotate(RobotPart p, double s): part(p), speed(s) {}
    };
    struct RotateTo
    {
        RobotPart part;
        double speed;
        double angle;
        RotateTo(RobotPart p, double s, double a): part(p), speed(s), angle(a) {}
    };
    struct RotateAmount
    {
        RobotPart part;
        double speed;
        double angle;
        RotateAmount(RobotPart p, double s, double a): part(p), speed(s), angle(a) {}
    };
    struct Sweep
    {
        RobotPart part;
        double speed;
        double left;
        double right;
        Sweep(RobotPart p, double s, double l, double r): part(p), speed(s), left(l), right(r) {}
    };
    struct Accelerate
    {
        double acc;
        Accelerate(double a): acc(a) {}
    };
}

#endif
