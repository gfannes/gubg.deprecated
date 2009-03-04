#ifndef cubeFunction_h
#define cubeFunction_h

#include <vector>

#include "function.hpp"
#include "math.hpp"
#include "cube.hpp"

using namespace std;
using namespace Math;

// This class creates a function from a matrix by interpolation of its values to get those for coordinates in between cells
// The matrix is transformed to a function as is (rix = yco, cix = xco), but with the cell [0,0] in (x,y) and the cells [nrR-1,nrC-1] at coordinate (x+w,y+h)
template <typename T>
class MatrixFunction: public Function<double,0,1>
{
public:
    typedef vector<vector<T> > DataType;
    enum BoundaryType
    {
        eFixedValue,
        eRepeatBoundary,
        eRepeatMatrix
    };

    // a and b are the transformation to be applied to compute an output based on the pixel value
    MatrixFunction(DataType *data, double x = 0, double y = 0, double w = 1, double h = 1, double a = 1, double b = 0, BoundaryType boundaryType = eFixedValue, double fixedValue = 0.0):
        mX(x),
        mY(y),
        mW(w),
        mH(h),
        mA(a),
        mB(b),
        mBoundaryType(boundaryType),
        mFixedValue(fixedValue)
        {
            setMatrix(data);
        }
    void getLocation(double &x, double &y, double &w, double &h)
        {
            x = mX;
            y = mY;
            w = mW;
            h = mH;
        };

    bool setMatrix(DataType *data)
        {
            mNrR = data->size();
            if (mNrR == 0)
                cerr << "No valid matrix data found" << endl;

            mNrC = (*data)[0].size();

            computeLinTrans(mAX,mBX,mX,0.0,mX+mW,mNrC-1);
            computeLinTrans(mAY,mBY,mY,0.0,mY+mH,mNrR-1);
            
            // The corners
            mLB = (*data)[0][0];
            mLT = (*data)[0][mNrR-1];
            mRB = (*data)[mNrC-1][0];
            mRT = (*data)[mNrC-1][mNrR-1];

            // The center interpolation
            if (mBoundaryType == eFixedValue)
            {
                mCenter.resize(mNrR+1);
                // Bottom row of interpols going to mFixedValue
                mCenter[0].resize(mNrC+1);
                mCenter[0][0].set(mFixedValue,mFixedValue,mFixedValue,mLB);
                for (int jj=0;jj<mNrC-1;++jj)
                    mCenter[0][jj+1].set(mFixedValue,mFixedValue,(*data)[0][jj],(*data)[0][jj+1]);
                mCenter[0][mNrC].set(mFixedValue,mFixedValue,mRB,mFixedValue);
                for (int i=0;i<mNrR-1;++i)
                {
                    mCenter[i+1].resize(mNrC+1);
                    // mFixedValue to Left
                    mCenter[i+1][0].set(mFixedValue,(*data)[i][0],mFixedValue,(*data)[i+1][0]);
                    for (int j=0;j<mNrC-1;++j)
                        mCenter[i+1][j+1].set((*data)[i][j],(*data)[i][j+1],(*data)[i+1][j],(*data)[i+1][j+1]);
                    // Right to mFixedValue
                    mCenter[i+1][mNrC].set((*data)[i][mNrC-1],mFixedValue,(*data)[i+1][mNrC-1],mFixedValue);
                }
                // Top row of interpols going to mFixedValue
                mCenter[mNrR].resize(mNrC+1);
                mCenter[mNrR][0].set(mFixedValue,mLT,mFixedValue,mFixedValue);
                for (int jj=0;jj<mNrC-1;++jj)
                    mCenter[mNrR][jj+1].set((*data)[mNrR-1][jj],(*data)[mNrR-1][jj+1],mFixedValue,mFixedValue);
                mCenter[mNrR][mNrC].set(mRT,mFixedValue,mFixedValue,mFixedValue);
            } else
            {
                if (mNrC > 1 && mNrR > 1)
                {
                    mCenter.resize(mNrR-1);
                    for (int i=0;i<mNrR-1;++i)
                    {
                        mCenter[i].resize(mNrC-1);
                        for (int j=0;j<mNrC-1;++j)
                            mCenter[i][j].set((*data)[i][j],(*data)[i][j+1],(*data)[i+1][j],(*data)[i+1][j+1]);
                    }
                }
            }

            // The boundary interpolations
            if (mBoundaryType == eRepeatBoundary)
            {
                if (mNrR > 1)
                {
                    mLeft.resize(mNrR-1);
                    mRight.resize(mNrR-1);
                    for (int i=0;i<mNrR-1;++i)
                    {
                        mLeft[i].set((*data)[i][0],(*data)[i+1][0]);
                        mRight[i].set((*data)[i][mNrC-1],(*data)[i+1][mNrC-1]);
                    }
                }
                if (mNrC > 1)
                {
                    mBottom.resize(mNrC-1);
                    mTop.resize(mNrC-1);
                    for (int i=0;i<mNrC-1;++i)
                    {
                        mBottom[i].set((*data)[0][i],(*data)[0][i+1]);
                        mTop[i].set((*data)[mNrR-1][i],(*data)[mNrR-1][i+1]);
                    }
                }
            }
                
            return true;
        }
    virtual bool computeOutput()
        {
            if (!this->mpInput || this->mpInput->size()!=2)
            {
                cerr << "No valid input passed" << endl;
                return false;
            }
//            cout << "mpInput in matrixFunction = " << this->mpInput << "" << endl;
            double x = (*this->mpInput)[0],y = (*this->mpInput)[1];

            double dcix,drix;
            linTrans(dcix,x,mAX,mBX);
            linTrans(drix,y,mAY,mBY);
//             drix -= 0.5;
//             dcix -= 0.5;
            int rix = (int)drix,cix = (int)dcix;
            if (drix<0)
                --rix;
            if (dcix<0)
                --cix;
            double unitY = (drix-rix), unitX = (dcix-cix);
            switch (mBoundaryType)
            {
            case eRepeatMatrix:
                rix %= mNrR-1;
                if (rix < 0)
                    rix += mNrR-1;
                cix %= mNrC-1;
                if (cix < 0)
                    cix += mNrC-1;
                this->mOutput = mCenter[rix][cix].output(unitX,unitY);
                break;

            case eFixedValue:
                if (cix < -1 || cix >= mNrC || rix < -1 || rix >= mNrR)
                {
                    this->mOutput = mFixedValue;
                } else
                {
                    this->mOutput = mCenter[rix+1][cix+1].output(unitX,unitY);
                }
                break;

            case eRepeatBoundary:
                if (cix < 0)
                {
                    // X is left
                    if (rix < 0)
                    {
                        // Y is low
                        this->mOutput = mLB;
                    } else if (rix >= mNrR-1)
                    {
                        // Y is up
                        this->mOutput = mLT;
                    } else
                    {
                        // Y is in center
                        this->mOutput = mLeft[rix].output(unitY);
                    }
                } else if (cix >= mNrC-1)
                {
                    // X is right
                    if (rix < 0)
                    {
                        // Y is low
                        this->mOutput = mRB;
                    } else if (rix >= mNrR-1)
                    {
                        // Y is up
                        this->mOutput = mRT;
                    } else
                    {
                        // Y is in center
                        this->mOutput = mRight[rix].output(unitY);
                    }
                } else
                {
                    // X is in center
                    if (rix < 0)
                    {
                        // Y is low
                        this->mOutput = mBottom[cix].output(unitX);
                    } else if (rix >= mNrR-1)
                    {
                        // Y is up
                        this->mOutput = mTop[cix].output(unitX);
                    } else
                    {
                        // Y is in center
                        this->mOutput = mCenter[rix][cix].output(unitX,unitY);
                    }
                }
                break;
            }
            linTrans(this->mOutput,this->mOutput,mA,mB);
            return true;
        }
    virtual bool clone(Function<double,0,1> *&res) const
        {
            return res = new MatrixFunction(*this);
        }
private:
    // The various interpolation factors involved
    vector<vector<UnitSquareInterpolation> > mCenter;
    vector<UnitSegmentInterpolation> mLeft;
    vector<UnitSegmentInterpolation> mRight;
    vector<UnitSegmentInterpolation> mTop;
    vector<UnitSegmentInterpolation> mBottom;
    double mLB;
    double mLT;
    double mRB;
    double mRT;

    double mX;
    double mY;
    double mW;
    double mH;
    double mA;
    double mB;
    int mNrR;
    int mNrC;
    double mAX;
    double mBX;
    double mAY;
    double mBY;

    BoundaryType mBoundaryType;
    double mFixedValue;
};

#endif
