module gubg.GaussianPosterior;

import gubg.Array;
import gubg.Vector;
import gubg.Matrix;
import tango.math.Math;

//This class encapsulates the posterior distribution of the mean (and optionally a precision matrix (inverse of the covariance matrix)) of a Gaussian distribution
//when its prior distribution is Gaussion for the mean and Wishart for the precision matrix (if not known)
class GaussianPosterior(uint Dimension)
{
    this (real priorNrObs, real[] priorMean, real dataDecayFactor = 0.0)
    {
        priorNrObs_ = priorNrObs;
        priorMean_[] = priorMean[];
        dataNrObs_ = 0;
        setSame!(real)(dataMean_, 0.0);
        dataDecayFactor_ = dataDecayFactor;
    }

    void setPrecision()
    {
        precision_ = Matrix.identity(Dimension);
    }

    real logDensity(real[] mean)
    {
        check_;
        scope diff = new real[Dimension];
        real totalNrObs = 1.0 / (dataNrObs_ + priorNrObs_);
        foreach (ix, inout d; diff)
            d = totalNrObs * (dataNrObs_*dataMean_[ix] + priorNrObs_*priorMean_[ix]);
        add(diff, mean, -1.0); 
        return -0.5*Dimension*log(2*PI) + 0.5*log(precision_.determinant) - 0.5*precision_.leftRightProduct(diff);
    }
    real density(real[] mean)
    {
        return exp(logDensity(mean));
    }

    void update(real[] x, real dT = 0.0)
    {
        real downScaledDataNrObs = dataNrObs_;
        if (0.0 != dataDecayFactor_)
            downScaledDataNrObs *= exp(-dT*dataDecayFactor_);
        dataNrObs_ = downScaledDataNrObs + 1.0;
        foreach (ix, inout dm; dataMean_)
            dm = (x[ix] + downScaledDataNrObs*dm) / dataNrObs_;
    }
    void updateWithWeight(real[] x, real weight, real dT = 0.0)
    {
        real downScaledDataNrObs = dataNrObs_;
        if (0.0 != dataDecayFactor_)
            downScaledDataNrObs *= exp(-dT*dataDecayFactor_);
        dataNrObs_ = downScaledDataNrObs + weight;
        foreach (ix, inout dm; dataMean_)
            dm = (weight*x[ix] + downScaledDataNrObs*dm) / dataNrObs_;
    }
    //Update the posterior with a new time difference. No observation was made
    //dT: amount of time passed since last update
    void updateTime(real dT)
    {
        if (0.0 != dataDecayFactor_)
            dataNrObs_ = exp(-dT*dataDecayFactor_)*dataNrObs_;
     }


    private:
    //Consistency check
    void check_()
    {
        if (!precision_)
            throw new Exception("No precision matrix was specified.");
    }

    real priorNrObs_;
    real[Dimension] priorMean_;
    real dataNrObs_;
    real[Dimension] dataMean_;
    Matrix precision_;
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
        auto posterior = new GaussianPosterior!(2)(5, [1.0, 2.0], 0.2);
        posterior.setPrecision();

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
                return true;
            }

            //Show time
            visu.show(false, &update, 0.01);
        }
    }
}
