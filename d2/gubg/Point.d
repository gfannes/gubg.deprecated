module gubg.Point;

import gubg.Format;
import std.math;

struct Point
{
    this (real xx, real yy)
    {
        x = xx;
        y = yy;
    }
    this (real[] co)
    {
        this (co[0], co[1]);
    }
    real x;
    real y;

    string toString() const {return Format.immediate("(%s, %s)", x, y);}

    Point opBinary(string op)(Point rhs) const if ("+" == op)
    {
        Point res = this;
        res.x += rhs.x;
        res.y += rhs.y;
        return res;
    }
    Point opBinary(string op)(Point rhs) const if ("-" == op)
    {
        Point res = this;
        res.x -= rhs.x;
        res.y -= rhs.y;
        return res;
    }
    void multiplyScalar(real v)
    {
        x *= v;
        y *= v;
    }
    void multiplyByElement(in Point vs)
    {
        x *= vs.x;
        y *= vs.y;
    }
    void divideByElement(in Point vs)
    {
        x /= vs.x;
        y /= vs.y;
    }
}

struct TwoPoint
{
    this (real x0, real y0, real x1, real y1)
    {
        p0 = Point(x0, y0);
        p1 = Point(x1, y1);
    }
    this (real[] co0, real[] co1)
    {
        this (co0[0], co0[1], co1[0], co1[1]);
    }
    static TwoPoint centered(real x, real y, real w, real h)
    {
        w *= 0.5;
        h *= 0.5;
        return TwoPoint(x-w, y-h, x+w, y+h);
    }
    static TwoPoint centered(Point center, real w, real h)
    {
        return TwoPoint.centered(center.x, center.y, w, h);
    }
    Point p0;
    Point p1;

    //L2 distance
    real length() const {return sqrt(pow(p0.x-p1.x, 2) + pow(p0.y-p1.y, 2));}
    real width() const {return abs(moveX());}
    real height() const {return abs(moveY());}
    real centerX() const {return 0.5*(p0.x+p1.x);}
    real centerY() const {return 0.5*(p0.y+p1.y);}
    //The distance to move across a certain axis to go from p0 to p1
    real moveX() const {return p1.x-p0.x;}
    real moveY() const {return p1.y-p0.y;}
    bool isInside(in Point p) const
    {
        //Check the X-axis
        if (p0.x <= p1.x)
        {
            if (p.x < p0.x || p1.x < p.x)
                return false;
        } else
        {
            if (p.x < p1.x || p0.x < p.x)
                return false;
        }
        //Check the Y-axis
        if (p0.y <= p1.y)
        {
            if (p.y < p0.y || p1.y < p.y)
                return false;
        } else
        {
            if (p.y < p1.y || p0.y < p.y)
                return false;
        }
        //All axis match => p is inside the rectangle
        return true;
    }

    string toString() const {return Format.immediate("(%s, %s)", p0.toString, p1.toString);}
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        auto p = Point(1.1, 2.1);
        writeln(p.toString);
        auto rect = TwoPoint(1,4,3,2);
        writeln(rect.toString);
        writefln("Length = %s", rect.length);
        writefln("Height = %s, width = %s", rect.height, rect.width);
        writefln("MoveX = %s, moveY = %s", rect.moveX, rect.moveY);
        writefln("%s is in %s => %s", p, rect, rect.isInside(p));
        p.x = 0.9;
        writefln("%s is in %s => %s", p, rect, rect.isInside(p));
        rect = TwoPoint([10,20], [3,4]);
        writeln(rect.toString);
        writefln("p + p = %s", p+p);
        writefln("p - p = %s", p-p);
    }
}

