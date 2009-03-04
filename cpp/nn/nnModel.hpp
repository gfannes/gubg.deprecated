#include "mlp.hpp"
#include "posteriorLogProb.hpp"
#include "optimalization.hpp"
#include "vectar.hpp"

typedef MLP<double> MLPT;

class Model: public Distribution<vector<double> >
{
public:
    Model():
        mSize(-1.0),
        mMFLearn(NULL),
        mMFVisu(NULL),
        mMLP(NULL),
        mNrL1(-1),
        mNrL2(-1),
        mNrNeurons(-1){}
    virtual ~Model()
        {
            if (mMFLearn)
                delete mMFLearn;
            if (mMFVisu)
                delete mMFVisu;
            if (mMLP)
                delete mMLP;
        }

    void set(vector<vector<double> > &input, ConvexRegion &region, double x, double y, double w, double h, double size, int nrL1 = 5, int nrL2 = 2, int nrNeurons = 1)
        {
            mSize = size;
            mNrL1 = nrL1;
            mNrL2 = nrL2;
            mNrNeurons = nrNeurons;

            mMFLearn = new MatrixFunction<double>(&input, 0, 0, 1, 1, 0.95, 0);
            mMFVisu = new MatrixFunction<double>(&input, x, y, w, h, 0.95, 0);
            vector<int> nrPerLayer;
            Vector::set(nrPerLayer, mNrNeurons, 1);
            vector<NeuronE> typePerLayer;
            Vector::set(typePerLayer, eNeuronTanh, eNeuronSigmoid);
            mMLP = new MLPT(mNrL1*mNrL1, nrPerLayer, typePerLayer);
            if (!sParameter)
                sParameter = mMLP->generateParameter();
            Vector::setSame(mInput, mNrL1*mNrL1, 0.0);
            mMLP->setInputParameter(mInput, *sParameter);
            mX = x;
            mY = y;
            mW = w;
            mH = h;
            ConvexRegion::Point p;
            do
            {
                p.x = mLX = Random::drawUniform();
                p.y = mLY = Random::drawUniform();
            } while (!region.isInternal(p));
        }
    void show(DataVisu &datavisu)
        {
            datavisu.add2DFunction(mX,mY,mX+mW,mY+mH,*mMFVisu);
            datavisu.addRectangle(mX+mLX-mSize/2,mY+mLY-mSize/2,mSize,mSize,Color::Green());
        }
    double logDensity(vector<double> &loc)
        {
            setLocation(loc);
            double m = prediction();
            m = 100*(m-1);
            m -= (0.5-loc[0])*(0.5-loc[0]) + (0.5-loc[1])*(0.5-loc[1]);
            return m;
        }
    double prediction()
        {
            double s1, s2;
            s1 = mSize*(mNrL1-1)/mNrL1;
            s2 = mSize/mNrL1;
            MeanVectar<MatrixFunction<double> > vectar(mMFLearn, mLocation[0], mLocation[1], s1, s2, mNrL1, mNrL2);
            vectar.generate(mInput);
//             Vector::computeMean(m, mInput);
            return mMLP->output(mInput);
        }
    void setLocation(vector<double> &loc)
        {
            mLocation = loc;
        }

    MatrixFunction<double> *mMFLearn; // MatrixFunction that will be used for learning (in 01 square)
    MatrixFunction<double> *mMFVisu;  // MatrixFunction that will be used for visualization (other placement)
    MLPT *mMLP;
    double mLX,mLY;
    double mX, mY, mW, mH;
    double mSize;
    int mNrL1;
    int mNrL2;
    int mNrNeurons;
    vector<double> mInput;
    vector<double> mLocation;
    static vector<vector<vector<double> > > *sParameter;
};

vector<vector<vector<double> > > *Model::sParameter = NULL;

class ProposalH: public ConditionalDistribution<vector<double>, vector<double> >
{
public:
    ProposalH(ConvexRegion &region, double sigma):
        mRegion(region),
        mSigma(sigma){}
    bool draw(vector<double> &value, const vector<double> &condition)
        {
            ConvexRegion::Point p;
            do
            {
                p.x = condition[0]+mSigma*Random::drawGaussian();
                p.y = condition[1]+mSigma*Random::drawGaussian();
            } while (!mRegion.isInternal(p));
            value.resize(2);
            value[0] = p.x;
            value[1] = p.y;
            return true;
        }
private:
    ConvexRegion &mRegion;
    double mSigma;
};
