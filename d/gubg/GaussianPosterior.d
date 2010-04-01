module gubg.GaussianPosterior;

//This class encapsulates the posterior distribution of the mean (and optionally a precision matrix (inverse of the covariance matrix)) of a Gaussian distribution
//when its prior distribution is Gaussion for the mean and Wishart for the precision matrix (if not known)
class GaussianPosterior(uint Dimension)
{
    this (real priorNrObs, real[] priorMean)
    {
        priorNrObs_ = priorNrObs;
        priorMean_[] = priorMean[];
    }

    real logDensity(real[] mean)
    {

    }

    private:
    real priorNrObs_;
    real[Dimension] priorMean_;
    real dataNrObs_;
    real[Dimension] dataMean_;
};

version (UnitTest)
{
    void main()
    {
        auto posterior = new GaussianPosterior!(2)(10, [1.0, 2.0]);
    }
}
