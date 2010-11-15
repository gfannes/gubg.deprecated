module gubg.Coordinate;

import gubg.Point;
import std.math;

//Coordinate transformations (translation and rotation)
class Transformation
{
    //newOrigCo: coordinates of new origin in old coordinates
    //newOneOne: coordinates of new (1,1) in old coordinates (rotation is not yet taken into account, fix this)
    this (Point newOrigCo, Point newOneOne, real angle = 0.0)
    {
        translation_ = new Translation(newOrigCo);
        scale_ = new Scale(newOneOne - newOrigCo);
        rotation_ = new Rotation(angle);
    }

    //Coordinate transformations
    void transformCoN2O(out Point oco, in Point nco)
    {
        rotation_.rotateN2O(oco, nco);
        scale_.scaleN2O(oco, oco);
        translation_.translateN2O(oco, oco);
    }
    void transformCoO2N(out Point nco, in Point oco)
    {
        translation_.translateO2N(nco, oco);
        rotation_.rotateO2N(nco, nco);
    }

    //Length transformations
    void transformLN2O(out Point olen, in Point nlen)
    {
        rotation_.rotateN2O(olen, nlen);
        scale_.scaleN2O(olen, olen);
    }

    class Translation
    {
        this (Point newOrigCo)
        {
            newOrigCo_ = newOrigCo;
        }
        void translateN2O(out Point oco, in Point nco)
        {
            oco = nco + newOrigCo_;
        }
        void translateO2N(out Point nco, in Point oco)
        {
            nco = oco - newOrigCo_;
        }
        private:
        Point newOrigCo_;
    }

    class Scale
    {
        this (Point newScales)
        {
            newScales_ = newScales;
        }
        void scaleN2O(out Point oco, in Point nco)
        {
            oco = nco;
            oco.multiplyByElement(newScales_);
        }
        void scaleO2N(out Point nco, in Point oco)
        {
            nco = oco;
            nco.divideByElement(newScales_);
        }
        private:
        Point newScales_;
    }

    class Rotation
    {
        this (real angle)
        {
            setAngle(angle);
        }

        void setAngle(real angle)
        {
            angle_ = angle;
            sin_ = sin(angle);
            cos_ = cos(angle);
        }

        void rotateN2O(out Point oco, in Point nco)
        {
            //We cache nco.x in a local variable, oco can be the same as nco
            auto x = nco.x;
            oco.x = x*cos_ - nco.y*sin_;
            oco.y = x*sin_ + nco.y*cos_;
        }
        void rotateO2N(out Point nco, in Point oco)
        {
            //We cache oco.x in a local variable, nco can be the same as oco
            auto x = oco.x;
            nco.x =  x*cos_ + oco.y*sin_;
            nco.y = -x*sin_ + oco.y*cos_;
        }
        private:
        real angle_;
        real sin_;
        real cos_;
    }

    private:
    Translation translation_;
    Scale scale_;
    Rotation rotation_;
}

version(UnitTest)
{
    import std.stdio;
    void main()
    {
        auto transf = new Transformation(Point(1,2 ), Point(2, 3), 0.2);

        {
            writeln("New -> old -> new1");
            auto n = Point(1, 0);
            Point o, n1;
            writefln("new: %s, old: %s, new1: %s", n, o, n1);
            transf.transformCoN2O(o, n);
            writefln("new: %s, old: %s, new1: %s", n, o, n1);
            transf.transformCoO2N(n1, o);
            writefln("new: %s, old: %s, new1: %s", n, o, n1);
        }

        {
            writeln("Old -> new -> old1");
            auto o = Point(1, 0);
            Point n, o1;
            writefln("old: %s, new: %s, old1: %s", o, n, o1);
            transf.transformCoO2N(n, o);
            writefln("old: %s, new: %s, old1: %s", o, n, o1);
            transf.transformCoN2O(o1, n);
            writefln("old: %s, new: %s, old1: %s", o, n, o1);
        }

        {
            auto p = Point(1, 0);
            writefln("p = %s", p);
            transf.transformCoN2O(p, p);
            writefln("p = %s", p);
            transf.transformCoO2N(p, p);
            writefln("p = %s", p);
        }
    }
}
