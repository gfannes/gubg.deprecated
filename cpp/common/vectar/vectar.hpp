#ifndef vectar_h
#define vectar_h

// A class that transforms something into a vector

#include "block.hpp"
#include "grid.hpp"

namespace gubg
{
    template <typename SourceT>
    class MeanVectar
    {
    public:
	MeanVectar(SourceT *source, double x, double y, double s1, double s2, long nr1, long nr2):
	    mSource(source)
	    {
		set(x, y, s1, s2, nr1, nr2);
	    }

	bool set(double x, double y, double s1, double s2, long nr1, long nr2)
	    {
		mX = x;
		mY = y;
		mS1 = s1;
		mS2 = s2;
		mNr1 = nr1;
		mNr2 = nr2;
		return true;
	    }

	// Level two block that will go over the level two grid and compute the mean of the output values of the source function
	class Block2: public Block<Grid>
	{
	public:
	    Block2(SourceT *source):
		mSource(source){}
	    ~Block2()
		{
		    mSource->invalidateInput();
		}
	    bool preYield()
		{
		    mMean = 0;
		    mInput.resize(2);
		}
	    bool yield(double x, double y)
		{
		    mInput[0] = x;
		    mInput[1] = y;
		    mMean += mSource->output(mInput);
		    return true;
		}
	    bool postYield()
		{
		    mMean /= index;
		}
	    double mMean;
	    vector<double> mInput;
	    SourceT *mSource;
	};

	// Level one block that will go over the level one grid to collect the array of inputs for e.g., the neural network
	class Block1: public Block<Grid>
	{
	public:
	    Block1(double s2, long nr2, SourceT *source):
		mS2(s2),
		mNr2(nr2),
		mSource(source){}
	    bool yield(double &r, double x, double y)
		{
		    Grid grid(x,y,mS2,mS2,mNr2,mNr2);
		    Block2 block(mSource);

		    grid.each(block);
		    r = block.mMean;

		    return true;
		}
	    double mS2;
	    long mNr2;
	    SourceT *mSource;
	};
	bool generate(vector<double> &res)
	    {
		Block1 block(mS2,mNr2,mSource);
		Grid grid(mX,mY,mS1,mS1,mNr1,mNr1);

		return grid.collect(res,block);
	    }
    private:
	SourceT *mSource;
	double mX,mY;
	double mS1,mS2;
	long mNr1,mNr2;
    };
}

#endif
