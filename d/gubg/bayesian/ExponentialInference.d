module gubg.baysian.ExponentialInference;

import tango.math.Math;
import tango.math.GammaFunction;

//This class encapsulates the posterior distribution of the lambda parameter of an exponential distribution
//when its prior distribution is given by a Gamma distribution itself (hense the term conjugate)
// * Prior distribution: Gamma(priorNrObs, priorSumObs)
//On arrival of (exponentially distributed) data observations, these prior parameters are updated
//but they remain Gamma-distributed
class ExponentialInference
{
    this(real priorNrObs, real priorSumObs, real dataDecayFactor = 0.0)
    {
        priorNrObs_ = priorNrObs;
        priorSumObs_ = priorSumObs;
        dataDecayFactor_ = dataDecayFactor;
        dataNrObs_ = 0.0;
        dataSumObs_ = 0.0;
    }

    real logDensity(real lambda)
    {
        real nrObs = priorNrObs_ + dataNrObs_;
        real sumObs = priorSumObs_ + dataSumObs_;
        return nrObs*log(sumObs) + (nrObs-1.0)*log(lambda) - lambda*sumObs - logGamma(nrObs);
    }
    real density(real lambda)
    {
        return exp(logDensity(lambda));
    }

    real mean()
    {
        return (priorNrObs_ + dataNrObs_) / (priorSumObs_ + dataSumObs_);
    }

    version (NotImplementedYet)
    {
        real entropy()
        {
        }
    }

    //Update the posterior with a new observation
    //dt: amount of time passed since last update
    void update(real x, real dT = 0.0)
    {
        if (0.0 == dataDecayFactor_)
        {
            dataNrObs_ += 1.0;
            dataSumObs_ += x;
        } else
        {
            real decay = exp(-dT*dataDecayFactor_);
            dataNrObs_ = decay*dataNrObs_ + 1.0;
            dataSumObs_ = decay*dataSumObs_ + x;
        }
    }
    void updateWithWeight(real x, real weight, real dT = 0.0)
    {
        if (0.0 == dataDecayFactor_)
        {
            dataNrObs_ += weight;
            dataSumObs_ += weight*x;
        } else
        {
            real decay = exp(-dT*dataDecayFactor_);
            dataNrObs_ = decay*dataNrObs_ + weight;
            dataSumObs_ = decay*dataSumObs_ + weight*x;
        }
    }
    //Update the posterior with a new time difference. No observation was made
    //dT: amount of time passed since last update
    void updateTime(real dT)
    {
        if (0.0 != dataDecayFactor_)
        {
            real decay = exp(-dT*dataDecayFactor_);
            dataNrObs_ = decay*dataNrObs_;
            dataSumObs_ = decay*dataSumObs_;
        }
    }

    private:
    real priorNrObs_;
    real priorSumObs_;
    real dataDecayFactor_;
    real dataNrObs_;
    real dataSumObs_;
}

version (UnitTest)
{
    import tango.core.Thread;
    import gubg.Puts;
    import gubg.Array;
    import gubg.graphics.Visu;
    void main()
    {
        auto posterior = new ExponentialInference(10, 10, 0.5);

        //Compute the density graph for posterior
        real[] xs, ys;
        spreadEqual(xs, 0.01, 3.0, 300);
        ys.length = xs.length;
        foreach (ix, inout y; ys)
            y = posterior.density(xs[ix]);

        //Create the visualization object
        real[2] minMaxX, minMaxY;
        minMax(minMaxX, xs);
        minMax(minMaxY, ys);

        {
            scope visu = Visu.create(640, 480, minMaxX, minMaxY);

            //Create the factory for creating a path
            scope of = new Factory(null, null);
            of.strokeColor(Color(0.5, 0.5, 0.5));

            //Create the path
            Path path;
            visu.add(path = of.createPath(xs, ys));

            visu.show(false, null, 0.1);
        }
    }
}
