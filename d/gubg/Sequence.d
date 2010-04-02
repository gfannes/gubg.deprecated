module gubg.Sequence;

import gubg.Math;
import gubg.Vector;
import gubg.Array;
import gubg.Puts;

import tango.math.Math;

//Ultimately, Sequence should be useful for detecting if a certain sequence is present.
//Such a sequence would be when each of the different input patterns occur externally on the inputs
// => Use GammaPosterior to keep information about decayFactor
// => Design a test for experimentation
//  * A simplified test where the inputs _are_ directly the pattern probs

//Interpretation:
// * Natural sequence of states is enforced/modelled with the rotation
// * Sequence is present if the input patterns occur sequentially
// * Detection of the sequence is gradual
//    * Entropy of the posterior of decayFactor
// * The better the sequence is detected, the better the input is followed?
// * Or, the better the sequence is detected, the better the prior is followed, which is more or less run by the posterior on defayFactor?
// * During execution, the posterior is continuously updated
// * 

// model: log(p_i*cte) = x_i
// cte = (p_0*...*p_n)^(1/n) => sum_j(x_j) = 0
// p_i = exp(x_i)/sum_j(exp(x_j))

// q(dT) = prob to stay in state during dT timespan
// q(dT) = q(1)^dT
// j(dT) = prob to jump to state j in timespan dT
// j(dT) = j(1)*(1-q(dT))/(1-q(1))

version = ExponentialDecay;

class Sequence
{
    this(uint nrStates, real rotationSpeed, real convergenceToPrior)
    {
        rotationSpeed_ = rotationSpeed;
        convergenceToPrior_ = convergenceToPrior;
        probs_ = new real[nrStates];
        rotations_ = new real[nrStates];
        foreach (ix, inout r; rotations_)
            r = cast(real)ix / nrStates;
        if (true)
            computeProbs_(probs_, 0);
        else
            foreach (ix, inout p; probs_)
                p = (ix == 40);
    }

    real[] probs(){return probs_;}

    uint nrStates(){return probs_.length;}

    void update(real dT)
    {
        //Compute the target probs
        auto targetProbs = new real[nrStates];
        scope(exit) delete targetProbs;
        auto rotation = computeRotation_ + rotationSpeed_*dT;
        computeProbs_(targetProbs, rotation);

        //Compute the distribution ratio between probs_ and targetProbs
        real dp = exp(-convergenceToPrior_*dT);
        real dt = 1.0-dp;

        //Shift the probs_ towards the targetProbs
        foreach (ix, inout p; probs_)
            p = dp*p + dt*targetProbs[ix];

        //Normalize the probs again (probably not really necessary)
        //probs_[] = targetProbs[];
        normalizeL1(probs_);
    }

    private:
    void computeProbs_(inout real[] ps, real rotation)
    {
        rotation %= 1.0;
        if (rotation < 0.0)
            rotation += 1.0;
        foreach (ix, inout p; ps)
        {
            //Create a W-shape with domain (-1 .. 1) and range (0 ..1)
            // * the normal abs() (the outer one) with slope 2
            // * shifting it to the right by 0.5 (the -1.0): A V-shape from (0 .. 1) to (0 .. 1)
            // * copying its behaviour to domain (-1 .. 0) (the inner abs)
            version (LinearDecay) p = abs(2.0*abs(rotation-rotations_[ix])-1.0);
            //Create an exponential-decay W shape by
            // * taking the linear W-shape shifted downwards (from (-1 .. 1) to (-1 .. 0))
            // * adjusting its slope with factor (from (-1 .. 1) to (-factor .. 0))
            // * taking the exp() from this (from (-1 .. 1) to (0 .. 1))
            version (ExponentialDecay)
            {
                const decayFactor = 5.0;
                p = exp(decayFactor*(abs(2.0*abs(rotation-rotations_[ix])-1.0)-1.0));
            }
        }
        normalizeL1(ps);
    }
    real computeRotation_()
    {
        creal sum = cast(creal)0.0;
        foreach (ix, p; probs_)
            sum += p * exp(rotations_[ix]*1i*2.0*PI);
        auto rotation = atan2(sum.im, sum.re) * 0.5 * M_1_PI;
        if (rotation < 0.0)
            rotation += 1.0;
        return rotation;
    }

    real[] probs_;
    real[] rotations_;
    real rotationSpeed_;
    real convergenceToPrior_;
}

version (UnitTest)
{
    import gubg.Timer;
    import gubg.graphics.Visu;
    import tango.core.Thread;

    void main()
    {
        const NrStates = 100;
        auto seq = new Sequence(NrStates, 5, 1.1);
        auto visu = new Visu(640, 480, [0,0], [600.0/NrStates, 480.0*NrStates/8]);
        //Factory used for creating squares and circles, we use no predefined stroke nor fill style
        auto of = new Factory(null, null);
        of.fillColor(Color(0.5, 0.5, 0.5));

        //Start the visualization thread
        visu.show(true, null, 0.01);
        Rectangle[NrStates] rectangles;
        foreach (ix, prob; seq.probs.dup)
            visu.add(rectangles[ix] = of.createRectangle([0.0+ix,0], [1.0+ix, prob]));

        auto timer = new Timer;
        auto timerSinceStart = new Timer(false);
        for (uint i = 0; i < 10000; ++i)
        {
            real dT = timer.difference;
            seq.update(dT);
            auto p = seq.probs.dup;
            auto d1 = abs(p[0]-p[NrStates-1]);
            auto d2 = abs(p[0]-p[1]);
            puts("Elapse = {}", timerSinceStart.difference);
            foreach (ix, prob; p)
                rectangles[ix].setCoordinates([0.0+ix,0], [1.0+ix, prob]);
            Thread.sleep(0.01);
        }
        visu.stop();
    }
}
