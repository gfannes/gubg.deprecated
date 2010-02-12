module gubg.Coordinate;

import tango.math.Math;

import gubg.Puts;

// Coordinate transformations (translation and rotation)
class Transformation
{
    // newOrigCo: coordinates of new origin in old coordinates
    // newOneOne: coordinates of new (1,1) in old coordinates (rotation is not yet taken into account, fix this)
    this(real[] newOrigCo, real[] newOneOne, real angle = 0.0)
    {
        translation_ = new Translation(newOrigCo);
        real[] scale;
        scale.length = newOrigCo.length;
        foreach (ix, o; newOrigCo)
            scale[ix] = newOneOne[ix] - o;
        scale_ = new Scale(scale);
        rotation_ = new Rotation(angle);
    }

    bool transformCoN2O(out real[] oco, in real[] nco)
    {
        return rotation_.rotateN2O(oco, nco) && scale_.scaleN2O(oco, oco) && translation_.translateN2O(oco, oco);
    }

    bool transformCoO2N(out real[] nco, in real[] oco)
    {
        return translation_.translateO2N(nco, oco) && rotation_.rotateO2N(nco, nco);
    }

    bool transformLN2O(out real[] oco, in real[] nco)
    {
        return rotation_.rotateN2O(oco, nco) && scale_.scaleN2O(oco, oco);
    }

    class Translation
    {
        this(real[] newOrigCo)
        {
            newOrigCo_ = newOrigCo;
        }
        bool translateN2O(out real[] oco, in real[] nco)
        {
            if (newOrigCo_.length != nco.length)
            {
                err("Length of coordinate vector and translation vector should be the same");
                return false;
            }
            oco.length = nco.length;
            foreach(ix, v; newOrigCo_)
                oco[ix] = nco[ix] + v;
            return true;
        }
        bool translateO2N(out real[] nco, in real[] oco)
        {
            if (newOrigCo_.length != oco.length)
            {
                err("Length of coordinate vector and translation vector should be the same");
                return false;
            }
            nco.length = oco.length;
            foreach(ix, v; newOrigCo_)
                nco[ix] = oco[ix] - v;
            return true;
        }
        private:
        real[] newOrigCo_;
    }

    class Scale
    {
        this(real[] newScales)
        {
            newScales_ = newScales;
        }
        bool scaleN2O(out real[] oco, in real[] nco)
        {
            if (newScales_.length != nco.length)
            {
                err("Length of coordinate vector and scale vector should be the same");
                return false;
            }
            oco.length = nco.length;
            foreach(ix, v; newScales_)
                oco[ix] = nco[ix] * v;
            return true;
        }
        bool scaleO2N(out real[] nco, in real[] oco)
        {
            if (newScales_.length != oco.length)
            {
                err("Length of coordinate vector and scale vector should be the same");
                return false;
            }
            nco.length = oco.length;
            foreach(ix, v; newScales_)
                nco[ix] = oco[ix] / v;
            return true;
        }
        private:
        real[] newScales_;
    }

    class Rotation
    {
        this(real angle)
        {
            setAngle(angle);
        }

        void setAngle(real angle)
        {
            angle_ = angle;
            sin_ = sin(angle);
            cos_ = cos(angle);
        }

        bool rotateN2O(out real[] oco, in real[] nco)
        {
            if (nco.length != 2)
            {
                err("Length of coordinate vector for rotation should be the 2");
                return false;
            }
            oco.length = 2;
            real x = nco[0];
            oco[0] = x*cos_ - nco[1]*sin_;
            oco[1] = x*sin_ + nco[1]*cos_;
            return true;
        }
        bool rotateO2N(out real[] nco, in real[] oco)
        {
            if (oco.length != 2)
            {
                err("Length of coordinate vector for rotation should be the 2");
                return false;
            }
            nco.length = 2;
            real x = oco[0];
            nco[0] =  x*cos_ + oco[1]*sin_;
            nco[1] = -x*sin_ + oco[1]*cos_;
            return true;
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
    void main()
    {
        auto transf = new Transformation([1,2], [2,3], 0.2);

        {
            puts("New -> old -> new");
            real[] co = [1,0], o;
            puts("{}",co);
            transf.transformCoN2O(o, co);
            puts("{}",co);
            transf.transformCoO2N(o, co);
            puts("{}",co);
        }

        {
            puts("Old -> new -> old");
            real[] co = [1,0], o;
            puts("{}",co);
            transf.transformCoO2N(o, co);
            puts("{}",co);
            transf.transformCoN2O(o, co);
            puts("{}",co);
        }
    }
}
