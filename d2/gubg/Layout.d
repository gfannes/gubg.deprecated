module gubg.Layout;

import gubg.Point;
import std.array;

enum Direction {BottomUp, TopDown, LeftToRight, RightToLeft}
class Box
{
    this (in TwoPoint a)
    {
        area = a;
    }
    TwoPoint area;

    void split(uint nr, in Direction direction)
    {
        if (!subAreas_.empty())
            throw new Exception("There are already sub areas present");
        subAreas_.length = nr;
        switch (direction)
        {
            case Direction.BottomUp:
                auto sh = area.height/nr;
                foreach (ix, ref sa; subAreas_)
                    sa = new Box(TwoPoint(area.p0.x, area.p0.y+ix*sh, area.p1.x, area.p0.y+(ix+1)*sh));
                break;
            case Direction.TopDown:
                auto sh = area.height/nr;
                foreach (ix, ref sa; subAreas_)
                    sa = new Box(TwoPoint(area.p0.x, area.p0.y+(nr-ix-1)*sh, area.p1.x, area.p0.y+(nr-ix)*sh));
                break;
            case Direction.LeftToRight:
                auto sw = area.width/nr;
                foreach (ix, ref sa; subAreas_)
                    sa = new Box(TwoPoint(area.p0.x+ix*sw, area.p0.y, area.p0.x+(ix+1)*sw, area.p1.y));
                break;
            case Direction.RightToLeft:
                auto sw = area.width/nr;
                foreach (ix, ref sa; subAreas_)
                    sa = new Box(TwoPoint(area.p0.x+(nr-ix-1)*sw, area.p0.y, area.p0.x+(nr-ix)*sw, area.p1.y));
                break;
        }
    }
    void split(real[] fracs, in Direction direction)
    {
        if (!subAreas_.empty())
            throw new Exception("There are already sub areas present");
        //Compute the lengths of the different parts
        auto parts = fracs.dup;
        {
            real len;
            switch (direction)
            {
                case Direction.BottomUp:
                case Direction.TopDown:
                    len = area.height;
                    break;
                case Direction.LeftToRight:
                case Direction.RightToLeft:
                    len = area.width;
                    break;
            }
            foreach (ix, ref part; parts)
                part = fracs[ix]*len;
        }

        subAreas_.length = fracs.length;
        real cumulative = 0.0;
        switch (direction)
        {
            case Direction.BottomUp:
                foreach (ix, ref sa; subAreas_)
                {
                    auto newCumul = cumulative+parts[ix];
                    sa = new Box(TwoPoint(area.p0.x, area.p0.y+cumulative, area.p1.x, area.p0.y+newCumul));
                    cumulative = newCumul;
                }
                break;
            case Direction.TopDown:
                foreach (ix, ref sa; subAreas_)
                {
                    auto newCumul = cumulative+parts[ix];
                    sa = new Box(TwoPoint(area.p0.x, area.p1.y-newCumul, area.p1.x, area.p1.y-cumulative));
                    cumulative = newCumul;
                }
                break;
            case Direction.LeftToRight:
                foreach (ix, ref sa; subAreas_)
                {
                    auto newCumul = cumulative+parts[ix];
                    sa = new Box(TwoPoint(area.p0.x+cumulative, area.p0.y, area.p0.x+newCumul, area.p1.y));
                    cumulative = newCumul;
                }
                break;
            case Direction.RightToLeft:
                foreach (ix, ref sa; subAreas_)
                {
                    auto newCumul = cumulative+parts[ix];
                    sa = new Box(TwoPoint(area.p1.x-newCumul, area.p0.y, area.p1.x-cumulative, area.p1.y));
                    cumulative = newCumul;
                }
                break;
        }
    }
    int opApply(int delegate(ref Box) dg)
    {
        foreach (ref area; subAreas_)
        {
            auto res = dg(area);
            if (res) return res;
        }
        return 0;
    }
    int opApply(int delegate(ref uint ix, ref Box) dg)
    {
        foreach (ix, ref area; subAreas_)
        {
            auto res = dg(ix, area);
            if (res) return res;
        }
        return 0;
    }
    Box opIndex(uint ix)
    {
        return subAreas_[ix];
    }

    private:
    Box[] subAreas_;
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        Direction[] directions = [Direction.BottomUp, Direction.TopDown, Direction.LeftToRight, Direction.RightToLeft];

        foreach (direction; directions)
        {
            {
                scope box = new Box(TwoPoint.centered(0, 0, 1, 1));
                writefln("split(uint) for %s", box.area);
                box.split(4, direction);
                foreach (sb; box)
                    writefln("%s", sb.area);
                writeln("");
            }
            {
                scope box = new Box(TwoPoint.centered(0, 0, 1, 1));
                writefln("split(real[]) for %s", box.area);
                box.split([0.1, 0.2, 0.3, 0.4], direction);
                foreach (sb; box)
                    writefln("%s", sb.area);
                writeln("");
            }
        }
    }
}
