#ifndef imageData_hpp
#define imageData_hpp

#include <iostream>
#include <vector>
#include <fstream>

#include "dir.hpp"
#include "serialize.hpp"
#include "image.hpp"
#include "matrixFunction.hpp"
#include "convexRegion.hpp"
#include "datavisu.hpp"
#include "reporting.hpp"

#include "nnModel.hpp"

using namespace std;

class ImageData
{
public:
    ImageData(string name, string dirGood = "", string dirBad = ""):
        mFileName(name),
        mDirGood(dirGood),
        mDirBad(dirBad){}
    bool create()
        {
            clear();

            // Load the good and bad images and keep track of the labels
            cout << "Loading images from " << mDirGood << " and " << mDirBad << "" << endl;
            for (int label=0; label<2; ++label)
            {
                string dirName = (label == 0 ? mDirGood : mDirBad);
                Dir dir(dirName);
                vector<string> imageFiles;
                dir.entries(&imageFiles, NULL, ".+\\.bmp");
                for (int i=0; i<imageFiles.size(); ++i)
                {
                    Image img(imageFiles[i]);
                    vector<vector<double> > imgDbl;
                    Cube::convert(imgDbl, img.data());
                    mImagesTrain.push_back(imgDbl);
                    mLabelsTrain.push_back(label);
                }
            }
            cout << "I found " << mImagesTrain.size() << " in total" << endl;

            // Transform them to 0-1 range
            cout << "Transforming value to the range 0-1" << endl;
            double dMin, dMax, a, b;
            Cube::findMinMax(dMin, dMax, mImagesTrain);
            const double eps = 0.000001;
            Math::computeLinTrans(a, b, dMin, eps, dMax, 1.0-eps);
            Cube::transformLinear(mImagesTrain, a, b);

            // Find their convex regions
            cout << "Searching for convex regions" << endl;
            DataVisu dv(400, 400);
            for (int i=0; i<mImagesTrain.size(); ++i)
            {
                cout << "" << i << " / " << mImagesTrain.size() << "" << endl;
                vector<vector<double> > &image = mImagesTrain[i];
                MatrixFunction<double> mf(&image);
                ConvexRegion cr;
                cr.learn(mf, 10, 1000, 0.02, 0.15);
                mRegionsTrain.push_back(cr);

//                 dv.add2DFunction(0, 0, 1, 1, mf);
//                 for (int j=0; j<cr.nrSegments(); ++j)
//                 {
//                     ConvexRegion::Point start, end;
//                     cr.getSegment(start, end, j);
//                     dv.addLine(start.x, start.y, end.x, end.y);
//                 }
//                 dv.show();
            }

            // Save images, target labels and convex regions
            cout << "Saving everything to " << mFileName << "" << endl;
            ofstream fo(mFileName.c_str() ,ios::binary);
            Serialize::serializeWithType(mImagesTrain, fo);
            Serialize::serializeWithType(mLabelsTrain, fo);
            Serialize::serializeWithType(mRegionsTrain, fo);
            fo.close();
        }
    // Only keeps the first and last nrFrontBack images, labels and regions
    bool load(int nrFrontBack = -1, double fracTest = 0.3)
        {
            clear();

            ifstream fi(mFileName.c_str(), ios::binary);
            vector<vector<vector<double> > > images;
            vector<int> labels;
            vector<ConvexRegion> regions;
            Serialize::unserializeWithType(images, fi);
            Serialize::unserializeWithType(labels, fi);
            Serialize::unserializeWithType(regions, fi);
            if (nrFrontBack >= 0)
            {
                if (2*nrFrontBack > images.size())
                {
                    cerr << "Not enough elements to use a front-back of " << nrFrontBack << "" << endl;
                    return false;
                }
                images.erase(images.begin()+nrFrontBack, images.end()-nrFrontBack);
                labels.erase(labels.begin()+nrFrontBack, labels.end()-nrFrontBack);
                regions.erase(regions.begin()+nrFrontBack, regions.end()-nrFrontBack);
            }

            BernoulliDistribution bd(fracTest);
            bool bTest;
            for (int i=0; i<images.size(); ++i)
            {
                bd.draw(bTest);
                if (bTest)
                {
                    mImagesTest.push_back(images[i]);
                    mLabelsTest.push_back(labels[i]);
                    mRegionsTest.push_back(regions[i]);
                } else
                {
                    mImagesTrain.push_back(images[i]);
                    mLabelsTrain.push_back(labels[i]);
                    mRegionsTrain.push_back(regions[i]);
                }
            }

            cout << "I loaded train: " << mImagesTrain.size() << " images, " << mLabelsTrain.size() << " labels and " << mRegionsTrain.size() << " regions" << endl;
            cout << "I loaded test: " << mImagesTest.size() << " images, " << mLabelsTest.size() << " labels and " << mRegionsTest.size() << " regions" << endl;
            return true;
        }

    void clear()
        {
            mImagesTrain.resize(0);
            mLabelsTrain.resize(0);
            mRegionsTrain.resize(0);
            mImagesTest.resize(0);
            mLabelsTest.resize(0);
            mRegionsTest.resize(0);
        }

    bool learn(const double size, const double sigma)
        {
            // Create the models
            vector<Model> models;
            int nrModels = mImagesTrain.size();
            cout << "I will learn based on " << nrModels << " models" << endl;
            int nrPerRowTarget = int((sqrt(nrModels*1.5)+1)*1.2);
            double rix0 = 0,cix0 = 0,rix1 = 0,cix1 = 0;
            const int nrL1 = 5, nrL2 = 2, nrNeurons = 10;
            models.resize(nrModels);
            for (int i=0;i<nrModels;i++)
            {
                models[i].set(mImagesTrain[i], mRegionsTrain[i], cix1, rix1, 1, 1, size, nrL1, nrL2, nrNeurons);
                cix1+=1.2;
                if (cix1 >= nrPerRowTarget)
                {
                    cix1 = 0;
                    rix1+=1.2;
                }
            }

            DataVisu dv(800, 800, false);
            dv.show(true);

            vector<double> location;
            for (int j=0;j<100;++j)
            {
                cout << "j = " << j << "" << endl;
                vector<vector<double> > inputs4NN;
                for (int i=0;i<nrModels;i++)
                {
                    ProposalH proposal(mRegionsTrain[i], sigma);
                    Metropolis<Model,ProposalH> metropolis(models[i], proposal);
                    Vector::set(location,models[i].mLX,models[i].mLY);
                    metropolis.run(location,100);
                    models[i].mLX = location[0];
                    models[i].mLY = location[1];
                    inputs4NN.push_back(models[i].mInput);
                    models[i].show(dv);
                }
                dv.redraw();

                MLPT &mlp = *models[0].mMLP;
                vector<vector<vector<double> > > &weights = *Model::sParameter;
                GaussianD<vector<double> > prior(0.0, 10.0);
                PosteriorLogProb<MLPT, vector<double>, int> plpMLP(mlp, inputs4NN, mLabelsTrain, &prior);
                plpMLP.setInput(weights);
                cout << "Output of NN before optimalization = " << plpMLP.output() << "" << endl;
                Optimizer<PosteriorLogProb<MLPT, vector<double>, int> > optimizerMLP(1);
                double v;
                optimizerMLP.optimize(weights, plpMLP, &v);
                plpMLP.setInput(weights);
                cout << "Output of NN optimalization = " << plpMLP.output() << " (v = " << v << ")" << endl;
                cout << weights << endl;

                vector<int> preds;
                preds.resize(nrModels);
                double threshhold = 0.5;
                for (int i=0; i<nrModels; ++i)
                    preds[i] = ((models[i].prediction() >= threshhold) ? 1 : 0);
                vector<vector<int> > confusion;
                Reporting::confusionCounts(confusion, mLabelsTrain, preds);
                cout << "\n\n" << confusion << "" << endl;
                double classRate = Reporting::classificationRate(mLabelsTrain, preds);
                cout << "Classification rate = " << classRate << "" << endl;

                vector<double> dpreds;
                dpreds.resize(nrModels);
                for (int i=0; i<nrModels; ++i)
                    dpreds[i] = models[i].prediction();
                cout << "AUC train = " << Reporting::areaUnderROC(mLabelsTrain, dpreds) << "" << endl;

                // Make the calculations on the test set
                Model model;
                preds.resize(0);
                dpreds.resize(0);
                for (int i=0; i<mImagesTest.size(); ++i)
                {
                    model.set(mImagesTest[i], mRegionsTest[i], -1.0, -1.0, -1.0, -1.0, size, nrL1, nrL2, nrNeurons);
                    ProposalH proposal(mRegionsTest[i], sigma);
                    Metropolis<Model,ProposalH> metropolis(model, proposal);
                    Vector::set(location, model.mLX,model.mLY);
                    metropolis.run(location, 100);
                    model.mLX = location[0];
                    model.mLY = location[1];

                    preds.push_back((model.prediction() >= threshhold) ? 1 : 0);
                    dpreds.push_back(model.prediction());
                }
                Reporting::confusionCounts(confusion, mLabelsTest, preds);
                cout << "\n\n" << confusion << "" << endl;
                classRate = Reporting::classificationRate(mLabelsTest, preds);
                cout << "Classification rate = " << classRate << "" << endl;
                cout << "AUC test  = " << Reporting::areaUnderROC(mLabelsTest, dpreds) << "" << endl;
            }
            dv.finishThread();
            
            return true;
        }
private:
    string mFileName;
    string mDirGood;
    string mDirBad;

    vector<vector<vector<double> > > mImagesTrain;
    vector<int> mLabelsTrain;
    vector<ConvexRegion> mRegionsTrain;
    vector<vector<vector<double> > > mImagesTest;
    vector<int> mLabelsTest;
    vector<ConvexRegion> mRegionsTest;
};

#endif
