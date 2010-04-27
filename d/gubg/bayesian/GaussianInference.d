module gubg.bayesian.GaussianInference;

import gubg.Array;
import gubg.Vector;
import gubg.Matrix;
import tango.math.Math;

//This class encapsulates the most basic Bayesian inference where:
// * Data is Gaussian distributed (independently)
// * The mean of this data distribution is _unknown_ => this is the parameter of interest
// * The covariance of this data is _known_ (this is dataCovar)
// * Our prior information for the mean of the data distribution is Gaussian with:
//    * Mean: priorMean
//    * Covariance: dataCovar/priorSampleSize
//    => The latter simplifies things but restricts our expressiveness in the multi-dimensional case
class GaussianInference(uint Dimension)
{
    this (IMatrix dataCovar, real[] priorMean, real priorSampleSize, real dataDecayFactor = 0.0)
    {
        invDataCovar_ = dataCovar.inverse();
        logDetDataCovar_ = log(dataCovar.determinant);
        priorMean_[] = priorMean[];
        priorSS_ = priorSampleSize;
        dataSS_ = 0;
        setSame!(real)(dataMean_, 0.0);
        dataDecayFactor_ = dataDecayFactor;
    }

    real logDensity(real[] mean)
    {
        real totalSS = priorSS_ + dataSS_;
        real[Dimension] difference;
        real factor = 1.0 / totalSS;
        foreach (ix, inout v; difference)
            v = factor * (priorSS_*priorMean_[ix] + dataSS_*dataMean_[ix]) - mean[ix];
        return 0.5*(-cast(real)(Dimension)*log(2*PI) +
                (logDetDataCovar_-log(totalSS)) +
                -totalSS*invDataCovar_.leftRightProduct(difference));
    }
    real density(real[] mean)
    {
        return exp(logDensity(mean));
    }

    void update(real[] x, real dT = 0.0)
    {
        real downScaledDataSS = dataSS_;
        if (0.0 != dataDecayFactor_)
            downScaledDataSS *= exp(-dT*dataDecayFactor_);
        dataSS_ = downScaledDataSS + 1.0;
        foreach (ix, inout dm; dataMean_)
            dm = (x[ix] + downScaledDataSS*dm) / dataSS_;
    }
    void updateWithWeight(real[] x, real weight, real dT = 0.0)
    {
        real downScaledDataSS = dataSS_;
        if (0.0 != dataDecayFactor_)
            downScaledDataSS *= exp(-dT*dataDecayFactor_);
        dataSS_ = downScaledDataSS + weight;
        foreach (ix, inout dm; dataMean_)
            dm = (weight*x[ix] + downScaledDataSS*dm) / dataSS_;
    }
    //Update the posterior with a new time difference. No observation was made
    //dT: amount of time passed since last update
    void updateTime(real dT)
    {
        if (0.0 != dataDecayFactor_)
            dataSS_ = exp(-dT*dataDecayFactor_)*dataSS_;
     }


    private:
    IMatrix invDataCovar_;
    real logDetDataCovar_;
    real[Dimension] priorMean_;
    real priorSS_;
    real dataSS_;
    real[Dimension] dataMean_;
    real dataDecayFactor_;
};

version (UnitTest)
{
    import gubg.Puts;
    import gubg.graphics.Visu;
    import gubg.Grid;
    import gubg.Timer;
    void main()
    {
        auto posterior = new GaussianInference!(2)(createIdentity(2), [1.0, 2.0], 5, 0.2);

        Color logDensityColor(real x, real y)
        {
            real logD = 5*posterior.density([x, y]);
            return Color(logD, logD, logD);
        }

        {
            scope visu = Visu.create(640, 480, [0, 5], [0, 5]);

            //Create a grid of rectangles to show the logDensity of the posterior
            const NrPerAxis = 100;
            auto gridOfRectangles = new Grid!(Rectangle)(NrPerAxis, NrPerAxis, [0, 5], [0, 5]);
            auto of = new Factory(null, null);
            {
                Rectangle createCell(real cx, real cy)
                {
                    of.fillColor(logDensityColor(cx, cy));
                    auto rectangle = of.createCenteredRectangle([cx, cy], [gridOfRectangles.subWidth, gridOfRectangles.subHeight]);
                    visu.add(rectangle);
                    return rectangle;
                }
                gridOfRectangles.createCells(&createCell);
            }

            //Callback that is called during visualization
            void redrawCells(real cx, real cy, inout Rectangle rectangle)
            {
                rectangle.fillStyle.fillColor(logDensityColor(cx, cy));
            }
            auto timer = new Timer;
            bool update()
            {
                real dT = timer.difference;
                if (visu.elapsedTime < 5)
                    posterior.update([4, 4], dT);
                else
                    posterior.updateTime(dT);

                gridOfRectangles.each(&redrawCells);
                return visu.elapsedTime < 15;
            }

            //Show time
            visu.show(false, &update, 0.01);
        }
    }
}
