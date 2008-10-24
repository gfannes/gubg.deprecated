#ifndef matrixMask_hpp
#define matrixMask_hpp

#include <iostream>

#include "vectorMask.hpp"
#include "distribution.hpp"
#include "metropolis.hpp"
#include "kernel.hpp"
#include "random.hpp"

using namespace std;

class MatrixMask
{
public:
    MatrixMask(){}        
    MatrixMask(int nr, const VectorMask &vm)
        {
            mRowMasks.resize(nr);
            for (int i=0; i<nr; ++i)
                mRowMasks[i] = vm;
        }
    template <typename T>
    bool filter(vector<T> &internal, vector<T> &external, const vector<vector<T> > &source, bool truncateBefore = true)
        {
            if (source.size() != mRowMasks.size())
            {
                cerr << "The size of the row masks is different than the number of rows you passed" << endl;
                return false;
            }
            if (truncateBefore)
            {
                internal.resize(0);
                external.resize(0);
            }
            for (int i=0; i<source.size(); ++i)
                mRowMasks[i].filter(internal, external, source[i], false);
            return true;
        }

    template <typename T>
    bool learn(const vector<vector<T> > &data, long nrIter = 1000)
        {
            int nrRows = data.size();
            if (nrRows <= 0)
            {
                cerr << "I need at least one row to learn a MatrixMask" << endl;
                return false;
            }
            int nrCols = data[0].size();
            if (nrCols <= 0)
            {
                cerr << "I need at least one column to learn a MatrixMask" << endl;
                return false;
            }
            mRowMasks.resize(nrRows);

            // Initialize the mask to a diamond shape, except if there are not enough rows
            if (nrRows < 3)
            {
                int start = nrCols/4, end = 3*nrCols/4;
                for (int i=0; i<nrRows; ++i)
                {
                    VectorMask vm(start, end);
                    mRowMasks[i] = vm;
                }
            } else
            {
                for (int i=0; i<nrRows/2; ++i)
                {
                    VectorMask vm(nrCols/2-i*nrCols/nrRows-1, nrCols/2+i*nrCols/nrRows);
                    mRowMasks[i] = vm;
                    mRowMasks[nrRows-1-i] = vm;
                }
                if (nrRows%2 == 1)
                {
                    VectorMask vm(0, nrCols-1);
                    mRowMasks[nrRows/2] = vm;
                }
            }

            L1Dist<T> l1Dist(data, nrRows*nrCols/10);
            Proposal proposal(nrRows, nrCols);
            
            Metropolis<L1Dist<T>, Proposal> metro(l1Dist, proposal, true);
            metro.run(*this, nrIter, true);
            
            return true;
        }

    void print()
        {
            for (int i=0; i<mRowMasks.size(); ++i)
            {
                cout << "MM row " << i << "\t";
                mRowMasks[i].print();
            }
        }

    VectorMask &getVectorMask(int i){return mRowMasks[i];}

private:
    template <typename T>
    class L1Dist: public Distribution<MatrixMask>
    {
    public:
        L1Dist(vector<vector<T> > data, int minCount):
            mData(data),
            mMinCount(minCount){}
        double logDensity(MatrixMask &location)
            {
                vector<T> internal, external;
                location.filter(internal, external, mData);
                double distance = 0.0;
                if (internal.size()>=mMinCount && external.size()>=mMinCount)
                {
                    mInternal.learn(internal);
                    mExternal.learn(external);
                    distance =  mInternal.l1Distance(mExternal, 100);
                } else
                    cout << "MESSAGE::Not enough elements in internal or external while learning the MatrixMask" << endl;
                return exp(distance*10);
            }
    private:
        KernelD<T> mExternal;
        KernelD<T> mInternal;
        vector<vector<T> > mData;
        int mMinCount;
    };

    class Proposal: public ConditionalDistribution<MatrixMask, MatrixMask>
    {
    public:
        Proposal(int nrRows, int nrCols):
            mNrRows(nrRows),
            mNrCols(nrCols){}
        bool draw(MatrixMask &value, MatrixMask &condition)
            {
                value = condition;
                VectorMask &vm = value.getVectorMask(Random::drawUniformDiscrete(0, mNrRows));
                vm.perturbate(mNrCols);
                return false;
            }
    private:
        int mNrRows;
        int mNrCols;
    };

    vector<VectorMask> mRowMasks;
};

#endif
