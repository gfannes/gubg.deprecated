module gubg.GaussianPosterior;

import gubg.Array;
import gubg.Vector;
import gubg.Matrix;
import tango.math.Math;

//This class encapsulates the posterior distribution of the mean (and optionally a precision matrix (inverse of the covariance matrix)) of a Gaussian distribution
//when its prior distribution is Gaussion for the mean and Wishart for the precision matrix (if not known)
class GaussianPosterior(uint Dimension)
{
    this (real priorNrObs, real[] priorMean)
    {
        priorNrObs_ = priorNrObs;
        priorMean_[] = priorMean[];
        dataNrObs_ = 0;
        setSame!(real)(dataMean_, 0.0);
    }

    void setPrecision()
    {
        precision_ = Matrix.identity(Dimension);
    }

    real logDensity(real[] mean)
    {
        check_;
        scope diff = new real[Dimension];
        diff[] = priorMean_[];
        add(diff, mean, -1.0); 
        return -0.5*Dimension*log(2*PI) + 0.5*log(precision_.determinant) - 0.5*precision_.leftRightProduct(diff);
    }
    real density(real[] mean)
    {
        return exp(logDensity(mean));
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
};

version (UnitTest)
{
    import gubg.Puts;
    import gubg.graphics.Visu;
    import gubg.Grid;
    void main()
    {
        auto posterior = new GaussianPosterior!(2)(10, [1.0, 2.0]);
        posterior.setPrecision();
        puts("logDensity = {}", posterior.logDensity([0.0, 0.0]));

        auto visu = Visu.create(640, 480, [0, 5], [0, 5]);
        visu.show(true, null, 0.01);
        //Create a grid of rectangles to show the logDensity of the posterior
        const NrPerAxis = 100;
        auto gridOfRectangles = new Grid!(Rectangle)(NrPerAxis, NrPerAxis, [0, 5], [0, 5]);
        auto of = new Factory(null, null);
        {
            Rectangle createCell(real cx, real cy)
            {
                real logD = 5*posterior.density([cx, cy]);
                of.fillColor(Color(logD, logD, logD));
                auto rectangle = of.createCenteredRectangle([cx, cy], [gridOfRectangles.subWidth, gridOfRectangles.subHeight]);
                visu.add(rectangle);
                return rectangle;
            }
            gridOfRectangles.createCells(&createCell);
        }
        Thread.sleep(20);
        visu.stop();
    }
}
