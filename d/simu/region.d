module simu.region;

interface Region
{
    bool update(ref real[] state, ulong elapsedTime, ulong timeDiff);
}

import tango.math.Math;

import gubg.math;
import gubg.visu;
import gubg.drawable;
import gubg.style;
import gubg.puts;

class Spin: Region
{
    const real two_pi = 2*PI;
    const real radiusBig = 0.8;
    const real radiusSmall = 0.05;

    this (int nr, real period, Visu visu)
    {
	mNr = nr;
	mPeriod = period;
	mCircles.length = mNr;
	visu.setStroke(false);
	visu.setFillColor(Color.Purple);
	eachCoordinate(delegate void(int ix, real x, real y)
		       {mCircles[ix] = visu.addCircle([x, y], radiusSmall);}
	    );
    }

    real angle(int ix){return two_pi*ix/mNr;}
    void eachCoordinate(void delegate(int ix, real x, real y) callback)
    {
	for (int ix = 0; ix < mNr; ++ix)
	{
	    real a = angle(ix);
	    callback(ix, radiusBig*cos(a), radiusBig*sin(a));
	}
    }

    bool update(ref real[] state, ulong elapsedTime, ulong timeDiff)
    {
	if (state.length < mNr)
	    state.length = mNr;
	real center = two_pi*elapsedTime/(mPeriod*1000000.0);
	real sigma = 0.3;
	foreach (ix, inout s; state[0 .. mNr])
	{
	    real l1dist = roundCyclic(center - angle(ix), two_pi);
	    s = exp(-0.5*pow(l1dist/sigma, 2));
	    mCircles[ix].style().setFillColor(Color(s));
	}
	return true;
    }
private:
    int mNr;
    real mPeriod;
    Drawable[] mCircles;
}

import gubg.hopfield;
import gubg.neuron;
import gubg.random;

class NN: Region
{
    const real side = 0.03;
    const real minDist = 1.0;
    const real weightLimit = 5;

    this (int nr, Spin spin, Visu visu)
    {
	mLearning = true;

	// Allocate the neurons for the hopfield network
	mHopfield = new Hopfield(nr);

	// Set the coordinates of the neurons and their visualization
	mCoordinates.length = nr;
	mSquares.length = nr;
	// Create the followers
	visu.setStroke(true);
	visu.setStrokeColor(Color.Black);
	visu.setFillColor(Color.Purple);
	int lIX;
	spin.eachCoordinate(
	    delegate void(int ix, real x, real y)
	    {
		lIX = ix;
		x *= 1.07;
		y *= 1.07;
		mCoordinates[lIX][] = [x, y];
		mSquares[lIX] = visu.addCenteredSquare([x, y], 3*side);
	    }
	    );
	mNrFollowers = (++lIX);
	// Create the rest
	visu.setStroke(false);
	for (; lIX < nr; ++lIX)
	{
	    real x = uniform(-1, 1);
	    real y = uniform(-1, 1);
	    mCoordinates[lIX][] = [x, y];
	    mSquares[lIX] = visu.addCenteredSquare([x, y], side);
	}

	// Create the structure
	mHopfield.eachNeuron
	    (delegate void(int ix1, Neuron n1)
	     {
		 mHopfield.eachNeuron
		     (delegate void(int ix2, Neuron n2)
		      {
			  if (l2Dist(mCoordinates[ix1], mCoordinates[ix2]) < minDist)
			  {
			      n1.addSynaps(n2, uniform(-weightLimit*0.5, weightLimit));
			      mLines ~= [visu.addLine(mCoordinates[ix1], mCoordinates[ix2])];
			  }
		      });
	     });
	puts("Number of synapses = {}", mHopfield.nrSynapses());
    }
    
    bool update(ref real[] state, ulong elapsedTime, ulong timeDiff)
    {
	real dt = timeDiff/1000000.0;
	real dtFade = dt*exp(-elapsedTime/10000000.0);

	// Set the targets for the followers
	if (mLearning)
	    foreach (ix, s; state[0 .. mNrFollowers])
		mHopfield[ix].setTarget(s);
//	mHopfield[0].print();

	// Update the weights to make each neuron achieve its targets
	if (mLearning)
	    mHopfield.updateWeights(dt);

	// Simulate the network with these weights
	mHopfield.updateValues(dt);

	// Compute new targets to make neurons help to achieve each other targets
	if (mLearning)
	    mHopfield.updateTargets(dt);

	mHopfield.eachNeuron(
	    delegate void(int ix, Neuron n)
	    {
		mSquares[ix].style().setFillColor(Color(n.output()));
	    }
	    );
	return true;
    }

    void setLearning(bool learning){mLearning = learning;}
private:
    int mNrFollowers;
    Hopfield mHopfield;
    real[2][] mCoordinates;
    Drawable[] mSquares;
    Drawable[] mLines;
    bool mLearning;
}
