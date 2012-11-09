#ifndef HEADER_gubg_reporting_reporting_hpp_ALREADY_INCLUDED
#define HEADER_gubg_reporting_reporting_hpp_ALREADY_INCLUDED

#include <iostream>
#include <vector>

#include "vector.hpp"

using namespace std;

namespace gubg
{
    namespace Reporting
    {
        double classificationRate(const vector<int> &reals, const vector<int> &preds)
        {
            if (reals.size() != preds.size())
            {
                cerr << "I expect the same number of reals as preds when computing the classification rate" << endl;
                return -1.0;
            }
            int nrOK = 0;
            for (int i=0; i<reals.size(); ++i)
                if (reals[i] == preds[i])
                    ++nrOK;
            return double(nrOK)/reals.size();
        }

        // The reals are in the rows, the preds in the cols
        template <typename T>
        bool confusionCounts(vector<vector<T> > &matrix, const vector<int> &reals, const vector<int> &preds)
        {
            if (reals.size() != preds.size())
            {
                cerr << "I expect the same number of reals as preds when computing the confusion matrix" << endl;
                return false;
            }
        
            int nrReals = 0, nrPreds = 0;
            matrix.resize(0);
            for (int i=0; i<reals.size(); ++i)
            {
                int r = reals[i];
                int p = preds[i];

                // Check if the values are valid
                if (r<0 || p<0)
                {
                    cerr << "Make sure your reals and preds are always positive when computing a confusion matrix (" << r << ", " << p << ")" << endl;
                    return false;
                }
            
                // Is this a new reals value?
                if (r >= nrReals)
                {
                    matrix.resize(r+1);
                    for (int j=nrReals; j<r+1; ++j)
                    {
                        matrix[j].resize(nrPreds);
                        for (int k=0; k<nrPreds; ++k)
                            matrix[j][k] = 0;
                    }
                    nrReals = r+1;
                }
            
                // Is this a new preds value
                if (p >= nrPreds)
                {
                    for (int j=0; j<nrReals; ++j)
                    {
                        matrix[j].resize(p+1);
                        for (int k=nrPreds; k<p+1; ++k)
                            matrix[j][k] = 0;
                    }
                    nrPreds = p+1;
                }

                // Increment the value
                ++matrix[r][p];
            }

            return true;
        }

        template <typename T>
        bool rocCurve(vector<T> &xs, vector<T> &ys, vector<int> reals, vector<T> preds)
        {
            if (reals.size() != preds.size())
            {
                cout << "I expect the sizes of reals and preds to be equal in rocCurve." << endl;
                return false;
            }
            long nr = reals.size();
        
            // Sort the preds from large to small
            Vector::sort(preds, reals, false);

            long tp = 0, fp = 0, tn = 0, fn = 0;
            xs.resize(0);
            ys.resize(0);

            // treshhold is strictly bigger than the biggest pred-value
            for (int i = 0; i<nr; ++i)
            {
                switch (reals[i])
                {
                case 0:
                    ++tn;
                    break;
                case 1:
                    ++fn;
                    break;
                default:
                    cerr << "Unknown real label (" << reals[i] << ") in rocCurve" << endl;
                    return false;
                    break;
                }
            }
            xs.push_back((T)fp/(fp+tn));
            ys.push_back((T)tp/(tp+fn));

            for (int i=0; i<nr; ++i)
            {
                switch (reals[i])
                {
                case 0:
                    ++fp;
                    --tn;
                    break;
                case 1:
                    ++tp;
                    --fn;
                    break;
                }
                xs.push_back((T)fp/(fp+tn));
                ys.push_back((T)tp/(tp+fn));
            }

            return true;
        }
    
        template <typename T>
        double areaUnderROC(const vector<int> &reals, const vector<T> &preds)
        {
            vector<T> xs, ys;
            rocCurve(xs, ys, reals, preds);

            T px = xs[0], py = ys[0];
            double area = 0.0;
            for (int i=1; i<xs.size(); ++i)
            {
                T x = xs[i], y = ys[i];
                if (x!=px)
                {
                    if (y!=py)
                        cout << "strange" << endl;
                    area += (x-px)*y;
                }
                px = x;
                py = y;
            }

            return area;
        }
    }
}

#endif
