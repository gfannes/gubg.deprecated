#ifndef basis_h
#define basis_h

#include <iostream>

#include "math.hpp"

using namespace std;

namespace gubg
{
    class Basis
    {
    public:
        Basis(){}
        // (xO,yO) are the screen coordinates of the virtual (0,0)
        // (xO+xW,yO+yW) are the screen coordinates of the virtual (1,1)
        Basis(double xO, double yO, double xW, double yW, int height){setBasis(xO,yO,xW,yW,height);}

        bool setBasis(double xO, double yO, double xW, double yW, int height)
            {
                yO = height-yO;
                yW = -yW;
                return Math::computeLinTrans(mAX,mBX,0,xO,1,xO+xW) &&
                    Math::computeLinTrans(mAY,mBY,0,yO,1,yO+yW) &&
                    Math::computeLinTrans(mRAX,mRBX,xO,0,xO+xW,1) &&
                    Math::computeLinTrans(mRAY,mRBY,yO,0,yO+yW,1);
            };
        void screen2Virtual(double &vx, double &vy, double sx, double sy)
            {
                Math::linTrans(vx,sx,mRAX,mRBX);
                Math::linTrans(vy,sy,mRAY,mRBY);
            }
        void virtual2Screen(double &sx, double &sy, double vx, double vy)
            {
                Math::linTrans(sx,vx,mAX,mBX);
                Math::linTrans(sy,vy,mAY,mBY);
            }
        void screenWH2VirtualWH(double &vw, double &vh, double sw, double sh)
            {
                vw = sw*mRAX;
                vh = -sh*mRAY;
            }
        void virtualWH2ScreenWH(double &sw, double &sh, double vw, double vh)
            {
                sw = vw*mAX;
                sh = -vh*mAY;
            }
        void print()
            {
                cout << "X = (" << mAX << "," << mBX << "), Y = (" << mAY << "," << mBY << "), RX = (" << mRAX << "," << mRBX << "), RY = (" << mRAY << "," << mRBY << ")" << endl;
            }
    private:
        double mAX;
        double mBX;
        double mAY;
        double mBY;
        double mRAX;
        double mRBX;
        double mRAY;
        double mRBY;
    };
};

#endif
