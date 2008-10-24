#include <iostream>

#include "dir.hpp"
#include "image.hpp"
#include "data.hpp"
#include "mlp.hpp"
#include "posteriorLogProb.hpp"
#include "optimalization.hpp"
#include "matrix.hpp"
#include "serialize.hpp"
#include "initialization.hpp"
#include "maskedFunction.hpp"
#include "cube.hpp"
#include "random.hpp"
#include "datavisu.hpp"
#include "matrixFunction.hpp"
#include "vectar.hpp"
#include "metropolis.hpp"

using namespace std;
using namespace Serialize;

enum eTask
{
    eCreateData,
    eCreateTestData,
    eTrainNN,
    eTrainMaskedNN,
    eTrainMetropolis
};

typedef MLP<double> MLPT;

class Model: public Distribution<vector<double> >
{
public:
    Model():
        mMF(NULL),
        mMLP(NULL),
        mNrL1(5),
        mNrL2(2){}
    virtual ~Model()
        {
            if (mMF)
                delete mMF;
            if (mMLP)
                delete mMLP;
        }

    void set(vector<vector<double> > &input, double x, double y, double w, double h)
        {
            mFactor = 3;
            mMF = new MatrixFunction<double>(&input, x, y, w, h, 0.95, 0);
            vector<int> nrPerLayer;
            Vector::set(nrPerLayer, 1);
            vector<NeuronE> typePerLayer;
            Vector::set(typePerLayer, eNeuronSigmoid);
            mMLP = new MLPT(mNrL1*mNrL1, nrPerLayer, typePerLayer);
            if (!sParameter)
            {
                sParameter = mMLP->generateParameter();
//                Cube::setSame(*sParameter, 0.5);
            }
            Vector::setSame(mInput, mNrL1*mNrL1, 0.0);
            mMLP->setInputParameter(mInput, *sParameter);
            mX = x;
            mY = y;
            mW = w;
            mH = h;
            mLX = mX+0.5;//Random::drawUniform()*mW;
            mLY = mY+0.5;//Random::drawUniform()*mH;

        }
    void show(DataVisu &datavisu)
        {
            datavisu.add2DFunction(mX,mY,mX+mW,mY+mH,*mMF);
            double w = mW/mFactor, h = mH/mFactor;
            datavisu.addRectangle(mLX-w/2,mLY-h/2,w,h,Color::Green());
        }
    double logDensity(vector<double> &loc)
        {
            MeanVectar<MatrixFunction<double> > vectar(mMF,loc[0],loc[1],mW/mFactor,mW/15, mNrL1, mNrL2);
            vectar.generate(mInput);
            double m;
//             Vector::computeMean(m, mInput);
            m = mMLP->output(mInput);
            m = 100*(m-1);
            double cx = mX+0.5*mW, cy = mY+0.5*mH;
            m -= (cx-loc[0])*(cx-loc[0]) + (cy-loc[1])*(cy-loc[1]);
            return m;
        }
    MatrixFunction<double> *mMF;
    MLPT *mMLP;
    double mX,mY,mW,mH;
    double mLX,mLY;
    double mFactor;
    int mNrL1;
    int mNrL2;
    vector<double> mInput;
    static vector<vector<vector<double> > > *sParameter;
};

vector<vector<vector<double> > > *Model::sParameter = NULL;

int main(int argc, char *argv[])
{
//     eTask task = eCreateData;
//     eTask task = eCreateTestData;
//     eTask task = eTrainNN;
//     eTask task = eTrainMaskedNN;
    eTask task = eTrainMetropolis;

#define dirRaw    "/home/gfannes/data/raw"
#define fnData    "/home/gfannes/data/data.tsv"

    bool bTest = true;
    string fnDataBinInputs,fnDataBinTargets;
    if (bTest)
    {
        fnDataBinInputs = "/home/gfannes/data/testDataInputs.bin";
        fnDataBinTargets = "/home/gfannes/data/testDataTargets.bin";
    } else
    {
        fnDataBinInputs = "/home/gfannes/data/dataInputs.bin";
        fnDataBinTargets = "/home/gfannes/data/dataTargets.bin";
    }

    if (task == eCreateData)
    {
        cout << "Creating data" << endl;
        Dir dir(dirRaw);
        vector<string> dirs;
        vector<string> files;
        dir.entries(NULL,&dirs);
        Data *data = NULL;
        long nrPixels;
        vector<vector<vector<double> > > inputs;
        vector<int> targets;
        // Load the data from the images
        for (int i=0;i<dirs.size();i++)
        {
            Regex reTT("_Train");
            int tt = reTT.match(dirs[i]);
            Regex reGB("Good_");
            int gb = reGB.match(dirs[i]);

            cout << "" << dirs[i] << "" << endl;
            Dir dir2(dirs[i]);
            dir2.entries(&files,NULL,"\\.bmp");
            cout << "" << files.size() << "" << endl;
            long nrFiles2Load = files.size();
            nrFiles2Load = 50;
            for (int j=0;j<nrFiles2Load;j++)
            {
                Image image(files[j]);
                vector<int> pixels;
                vector<vector<double> > input;
                Cube::flatten(pixels,image.data());
                Cube::convert(input,image.data());
                inputs.push_back(input);
                targets.push_back(gb);
                if (!data)
                {
                    cout << "" << image.toString() << "" << endl;
                    vector<string> fieldNames;
                    vector<FieldTypeT> fieldTypes;
                    nrPixels = pixels.size();
                    for (int k=0;k<nrPixels;k++)
                    {
                        ostringstream ostr;
                        ostr << "pixel_" << k;
                        fieldNames.push_back(ostr.str());
                        fieldTypes.push_back(eFieldTypeInteger);
                    }
                    // Finally, we add a field that indicates train(1)/test(0) and good(1)/bad(0)
                    fieldNames.push_back("TrainTest");
                    fieldNames.push_back("GoodBad");
                    fieldTypes.push_back(eFieldTypeInteger);
                    fieldTypes.push_back(eFieldTypeInteger);
                    data = new Data(fieldNames,fieldTypes);
                }
                // Add the train/test and good/bad values
                pixels.push_back(tt);
                pixels.push_back(gb);
                data->addRecord(pixels);
            }
        }
        // Standardize the data to mean == 0 and standard deviation == 1
        double mean,sigma;
        Cube::computeMeanSigma(mean,sigma,inputs);
        Cube::add2All(inputs,-mean);
        Cube::multiply2All(inputs,1.0/sigma);
        for (long i=0;i<nrPixels;i++)
        {
            vector<int> oldF;
            vector<double> newF;
            data->extractField(oldF,i);
            Vector::computeMeanSigma(mean,sigma,oldF);
            Vector::standardizeUsing(newF,oldF,mean,sigma);
            data->replaceField(eFieldTypeFloat,newF,i);
        }        
        // Save the data
        data->save(fnData);
        // Save the binary data
        ofstream fo;
        fo.open(fnDataBinInputs.c_str(), ios::out | ios::binary);
        assert(Serialize::serializeWithType(inputs,fo));
        fo.close();
        fo.open(fnDataBinTargets.c_str(), ios::out | ios::binary);
        assert(Serialize::serializeWithType(targets,fo));
        fo.close();
    }

    if (task == eCreateTestData)
    {
        int nrPerTarget = 50, dim = 12, spotDim = 4;
        Cube::Dimension<double,3>::type inputs;
        Cube::Dimension<int,1>::type targets;

        Matrix<double> spot(spotDim,spotDim);
        Cube::setSame(spot.data(),1.0);

        for (int target=0;target<=1;++target)
        {
            for (int i=0;i<nrPerTarget;++i)
            {
                Matrix<double> matrix(dim,dim);
                Cube::setSame(matrix.data(),0.0);
                if (target == 1)
                    matrix.setElements(Random::drawUniformDiscrete(-spotDim+1,dim),Random::drawUniformDiscrete(-spotDim+1,dim),spot);
                cout << "" << endl << "" << matrix << "" << endl;
                inputs.push_back(matrix);
                targets.push_back(target);
            }
        }
        // Save the binary data
        ofstream fo;
        fo.open(fnDataBinInputs.c_str(), ios::out | ios::binary);
        assert(Serialize::serializeWithType(inputs,fo));
        fo.close();
        fo.open(fnDataBinTargets.c_str(), ios::out | ios::binary);
        assert(Serialize::serializeWithType(targets,fo));
        fo.close();
    }

    if (task == eTrainNN)
    {
        int nrPixel = 2304;
        int targetIX = 2305;
        Data data(fnData);
        vector<vector<double> > inputs;
        vector<int> inputIXS;
        Vector::setIncremental(inputIXS,nrPixel,0,1);
        data.extract(inputs,inputIXS);
        vector<int> targets;
        data.extractField(targets,targetIX);

        vector<int> nrPerLayer;
        Vector::set(nrPerLayer,1);
        vector<NeuronE> typePerLayer;
        Vector::set(typePerLayer,eNeuronSigmoid);
        MLPT mlp(nrPixel,nrPerLayer,typePerLayer);
        vector<double> input;
        Vector::setSame(input,nrPixel,-2.0);
        MLPT::ParameterType *par;
        par=mlp.generateParameter();
        mlp.setInput(input);
        mlp.setParameter(*par);

        PosteriorLogProb<MLPT, vector<double>, int> plpMLP(mlp,inputs,targets);
        plpMLP.setInput(*par);
        cout << plpMLP.output() << endl;

        Optimizer<PosteriorLogProb<MLPT, vector<double>, int> > optimizerMLP;
        double v;
        optimizerMLP.optimize(*par,plpMLP,&v);
        cout << "optimize MLP parameters" << endl << toString(*par) << endl;
        cout << v << endl;
    }

    if (task == eTrainMaskedNN)
    {
        vector<vector<vector<double> > > inputs;
        vector<int> targets;
        ifstream fi;
        fi.open(fnDataBinInputs.c_str(), ios::in | ios::binary);
        assert(Serialize::unserializeWithType(inputs,fi));
        fi.close();
        cout << "I found " << inputs.size() << " input matrices" << endl;

        fi.open(fnDataBinTargets.c_str(), ios::in | ios::binary);
        assert(Serialize::unserializeWithType(targets,fi));
        fi.close();
        cout << "I found " << targets.size() << " targets" << endl;
        
        const int width = 10;
        const int height = 10;
        const int nrInputs = width*height;
        vector<bool> maskValues;
        Vector::setSame(maskValues,nrInputs,true);
        Matrix<bool> mask(width,height,maskValues);

        vector<int> nrPerLayer;
        Vector::set(nrPerLayer,1);
        vector<NeuronE> typePerLayer;
        Vector::set(typePerLayer,eNeuronSigmoid);
        MLPT mlp(nrInputs,nrPerLayer,typePerLayer);
        vector<double> input;
        Vector::setSame(input,nrInputs,-2.0);
        mlp.setInput(input);
        typedef MaskedFunction<double,0,2,3,MLPT> MaskFT;
        MaskFT maskedFunction(mlp,mask);

        MLPT::ParameterType *par;
        par=mlp.generateParameter();
        mlp.setParameter(*par);

        PosteriorLogProb<MaskFT, vector<vector<double> >, int> plpMLP(maskedFunction,inputs,targets);
        plpMLP.setInput(*par);
        cout << plpMLP.output() << endl;

        Optimizer<PosteriorLogProb<MaskFT, vector<vector<double> >, int> > optimizerMLP;
        double v;
        optimizerMLP.optimize(*par,plpMLP,&v);
        cout << "optimize MLP parameters" << endl << toString(*par) << endl;
        cout << v << endl;
    }

    if (task == eTrainMetropolis)
    {
        vector<vector<vector<double> > > inputs;
        vector<int> targets;
        ifstream fi;
        fi.open(fnDataBinInputs.c_str(), ios::in | ios::binary);
        assert(Serialize::unserializeWithType(inputs,fi));
        fi.close();
        cout << "I found " << inputs.size() << " input matrices" << endl;

        fi.open(fnDataBinTargets.c_str(), ios::in | ios::binary);
        assert(Serialize::unserializeWithType(targets,fi));
        fi.close();
        cout << "I found " << targets.size() << " targets" << endl;

        int nr = inputs.size(), nrR = 2*inputs[0].size(), nrC = 2*inputs[0][0].size();
        int nrPerRowTarget = int((sqrt(nr*1.5)+1)*1.2);

        DataVisu datavisu(2*(nrPerRowTarget*(nrC+5)),nrPerRowTarget*(nrR+5));

        // Create a Model per input picture
        vector<Model> ms;
        double rix0 = 0,cix0 = 0,rix1 = 0,cix1 = 0;
        ms.resize(nr);
        for (int i=0;i<nr;i++)
        {
            ms[i].set(inputs[i], cix1, rix1, 1, 1);
            cix1+=1.2;
            if (cix1 >= nrPerRowTarget)
            {
                cix1 = 0;
                rix1+=1.2;
            }
        }

        datavisu.show(true);
        vector<double> sigma;
        Vector::set(sigma,0.1,0.1);
        GaussianCD<vector<double> > cgd(sigma);
        for (int j=0;j<100;++j)
        {
            cout << "j = " << j << "" << endl;
            vector<vector<double> > inputs4NN;
            for (int i=0;i<nr;i++)
            {
                Metropolis<Model,GaussianCD<vector<double> > > metropolis(ms[i], cgd);
                Vector::set(sigma,ms[i].mLX,ms[i].mLY);
                metropolis.run(sigma,100);
                ms[i].mLX = sigma[0];
                ms[i].mLY = sigma[1];
                inputs4NN.push_back(ms[i].mInput);
                ms[i].show(datavisu);
            }
            datavisu.redraw();

            MLPT &mlp = *ms[0].mMLP;
            vector<vector<vector<double> > > &weights = *Model::sParameter;
            GaussianD<vector<double> > prior(0.0, 1.0);
            PosteriorLogProb<MLPT, vector<double>, int> plpMLP(mlp, inputs4NN, targets, &prior);
            plpMLP.setInput(weights);
            cout << "Output of NN before optimalization = " << plpMLP.output() << "" << endl;
            Optimizer<PosteriorLogProb<MLPT, vector<double>, int> > optimizerMLP(1);
            double v;
            optimizerMLP.optimize(weights, plpMLP, &v);
            plpMLP.setInput(weights);
            cout << "Output of NN optimalization = " << plpMLP.output() << " (v = " << v << ")" << endl;
            cout << weights << endl;
        }
        datavisu.finishThread();
    }
    return 0;
}
