module gubg.S;

import gubg.Math;
import gubg.Vector;
import gubg.Array;
import gubg.Puts;

import tango.math.Math;

// model: log(p_i*cte) = x_i
// cte = (p_0*...*p_n)^(1/n) => sum_j(x_j) = 0
// p_i = exp(x_i)/sum_j(exp(x_j))

// q(dT) = prob to stay in state during dT timespan
// q(dT) = q(1)^dT
// j(dT) = prob to jump to state j in timespan dT
// j(dT) = j(1)*(1-q(dT))/(1-q(1))

class Sequence
{
    this(uint nr, real rotation)
    {
        probs_ = new real[nr];
        rotations_ = new real[nr];
        foreach (ix, inout r; rotations_)
            r = cast(real)ix / nr;
        computeProbs_(probs_, rotation);
    }

    real[] probs(){return probs_;}

    uint nrStates(){return probs_.length;}

    void update(real dT)
    {
        auto oldProbs = new real[nrStates];
        scope(exit) delete oldProbs;
        oldProbs[] = probs_[];
        normalizeL1(probs_);

        auto rotation = computeRotation_ + 0.1*dT;
        computeProbs_(probs_, rotation);
    }

    private:
    void computeProbs_(inout real[] probs, real rotation)
    {
        foreach (ix, inout p; probs)
            p = abs(2*abs(rotation-rotations_[ix])-1.0);
        normalizeL1(probs);
    }
    real computeRotation_()
    {
        creal sum = cast(creal)0.0;
        foreach (ix, p; probs_)
            sum += p * exp(rotations_[ix]*1i*2*PI);
        auto rotation = atan2(sum.im, sum.re) * 0.5 * M_1_PI;
        if (rotation < 0.0)
            rotation += 1.0;
        return rotation;
    }

    real[] probs_;
    real[] rotations_;
}

version (UnitTest)
{
    import gubg.Timer;
    import gubg.graphics.Visu;
    import tango.core.Thread;

    void main()
    {
        const NrStates = 100;
        auto seq = new Sequence(NrStates, 0);
        auto visu = new Visu(640, 480, [0,0], [640.0/NrStates, 480.0*NrStates/5]);
        //Factory used for creating squares and circles, we use no predefined stroke nor fill style
        auto of = new Factory(null, null);
        of.fillColor(Color(0.5, 0.5, 0.5));

        //Start the visualization thread
        visu.show(true, null, 0.01);
        Rectangle[NrStates] rectanlges;
        foreach (ix, prob; seq.probs)
            visu.add(rectanlges[ix] = of.createRectangle([0.0+ix,0], [1.0+ix, prob]));

        auto timer = new Timer;
        for (uint i = 0; i < 10000; ++i)
        {
            real dT = timer.difference;
            seq.update(dT);
            auto p = seq.probs;
            //            puts("probs = {:g9}, {:g9}, {:g9}, ix = {}, dT = {:e9}", p[0], p[1], p[2], maxIndex(seq.probs), dT);
            foreach (ix, prob; p)
                rectanlges[ix].setCoordinates([0.0+ix,0], [1.0+ix, prob]);
            Thread.sleep(0.01);
        }
        visu.stop();
    }
}
