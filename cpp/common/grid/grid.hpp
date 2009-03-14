#ifndef grid_h
#define grid_h

#include <vector>

#include "block.hpp"
#include "math.hpp"

using namespace std;

namespace gubg
{
    class Grid
    {
    public:
	Grid(double x, double y, double w, double h, long nrX, long nrY, bool centered = true)
	    {
		set(x,y,w,h,nrX,nrY,centered);
	    }
	bool set(double x, double y, double w, double h, long nrX, long nrY, bool centered = true)
	    {
		mNrX = nrX;
		mNrY = nrY;
		if (centered)
		{
		    Math::computeLinTrans(mAX,mBX,0,x-0.5*w,nrX-1,x+0.5*w);
		    Math::computeLinTrans(mAY,mBY,0,y-0.5*h,nrY-1,y+0.5*h);
		} else
		{
		    Math::computeLinTrans(mAX,mBX,0,x,nrX-1,x+w);
		    Math::computeLinTrans(mAY,mBY,0,y,nrY-1,y+h);
		}
	    }

	template <typename BlockT>
	bool each(BlockT &block)
	    {
		bool ok = true;
		double y;

		vector<double> xs;
		xs.resize(mNrX);
		for (int j=0;ok && j<mNrX;++j)
		    Math::linTrans(xs[j],j,mAX,mBX);

		block.index = 0;
		block.context = this;
		block.preYield();
		for (int i=0;ok && i<mNrY;++i)
		{
		    Math::linTrans(y,i,mAY,mBY);
		    for (int j=0;ok && j<mNrX;++j,++block.index)
			ok = block.yield(xs[j],y);
		}
		return block.postYield();
	    }

	template <typename BlockT>
	bool collect(vector<double> &res, BlockT &block)
	    {
		res.resize(mNrX*mNrY);
            
		bool ok = true;
		double y, r;

		vector<double> xs;
		xs.resize(mNrX);
		for (int j=0;ok && j<mNrX;++j)
		    Math::linTrans(xs[j],j,mAX,mBX);

		block.index = 0;
		block.context = this;
		block.preYield();
		for (int i=0;ok && i<mNrY;++i)
		{
		    Math::linTrans(y,i,mAY,mBY);
		    for (int j=0;ok && j<mNrX;++j,++(block.index))
		    {
			ok = block.yield(r, xs[j], y);
			if (ok)
			    res[block.index] = r;
		    }
		}
		return block.postYield();
	    }
    private:
	long mNrX;
	long mNrY;
	double mAX;
	double mBX;
	double mAY;
	double mBY;
    };
}

#endif
