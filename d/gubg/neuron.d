module gubg.neuron;

import tango.math.Math;
import tango.math.IEEE;
import tango.stdc.stdlib: exit;

import gubg.puts;

class Neuron
{
    const real eps = 0.01;

    this(int ix, real bias)
        {
            mIX = ix;
	    setIV(0.0);
	    setTarget(0.0);
	    mBias = bias;
        }

    // Create a connection from neuron to this with weight
    bool addSynaps(Neuron neuron, real weight)
        {
            Synaps s = {neuron, weight};
            mSynapses ~= [s];
            return true;
        }

    // The external activation value
    real output(){return mEV;}

    // Update the value of this neuron
    void updateValue(real dt)
	{
	    real sumIncoming = mBias;
	    foreach (s; mSynapses)
		sumIncoming += s.neuron.output()*s.weight;

	    setIV(mIV + dt*(-mIV + sumIncoming));
	}

    // Update the weights of this neuron
    void updateWeights(real dt)
	{
	    // Update the weights in the direction of -d(0.5*(output - Target)^2)/dw multiplied with dt
	    real factor = -(mEV - mTarget)*((1.0 + mEV)*(1.0 - mEV));
	    if (mIX == 0)
	    {
		puts("factor = {}", factor);
		print();
	    }
	    foreach (inout s; mSynapses)
		s.weight += factor*s.neuron.output();
//	    mBias += factor;
	}

    // Computation of targets
    void providePartialTargets()
	{
	    if (mSynapses.length > 0)
	    {
		// Allocate the partial targets for distribution
		auto partialTargets = new real[mSynapses.length];
		
		// Compute the partial targets and add it
		real atanhTarget = atanh(mTarget);
		if (isNaN(atanhTarget))
		{
		    puts("target = {}", mTarget);
		    exit(-1);
		}
		foreach (ixPT, inout pt; partialTargets)
		{
		    pt = 0;
		    foreach (ixS, s; mSynapses)
			pt += (ixPT == ixS ? atanhTarget : -s.weight*s.neuron.output());
		    pt /= mSynapses[ixPT].weight + eps;
		    
		    mSynapses[ixPT].neuron.addPartialTarget(pt, abs(mSynapses[ixPT].weight));
		}
	    }
	}
    void addPartialTarget(real t, real absWeight)
	{
	    PartialTarget pt;
	    pt.target = t;
	    pt.weight = absWeight;
	    mPartialTargets ~= [pt];
	}
    void clearPartialTargets(){mPartialTargets.length = 0;}
    void updateTarget(real dt)
	{
	    real target = 0;
	    real totalAbsWeight = 0;
	    foreach (pt; mPartialTargets)
	    {
		target += pt.weight*truncateTarget(pt.target);
		totalAbsWeight += pt.weight;
	    }
	    target = truncateTarget(target/(totalAbsWeight + eps));
	    mTarget += dt*(target - mTarget);
	}    
    real truncateTarget(real target)
	{
	    const real limit = 1.0 - eps;
	    if (target > limit)
		return limit;
	    else if (target < -limit)
		return -limit;
	    return target;
	}
    void setTarget(real target){mTarget = truncateTarget(target);}

    void setIV(real iv)
	{
	    mIV = iv;
	    mEV = tanh(iv);
	}

    ulong nrSynapses(){return mSynapses.length;}

    // Output functions
    void print()
        {
            puts("Neuron {} has output = {}, target = {} and {} synapses:", mIX, mEV, mTarget, mSynapses.length);
            printSynapses();
        }
    void printSynapses()
        {
            foreach(s; mSynapses)
                putsn("{}: {} => {} | ", s.neuron.mIX, s.neuron.output(), s.weight);
	    puts("");
        }

private:
    struct Synaps
    {
        Neuron neuron;
        real weight;
    }
    struct PartialTarget
    {
	real target;
	real weight;
    }

    int mIX;
    real mIV;			// Internal value: x(t) in df: dx(t)/dt = -x(t) + W*tanh(x(t))
    real mEV;			// External value: visible to other Neurons
    real mTarget;		// Should always be in ]-1, 1[ since we compute atanh from this
    PartialTarget[] mPartialTargets;
    Synaps[] mSynapses;
    real mBias;
}

version (UnitTest)
{
    void main()
    {
        Neuron n = new Neuron;
    }
}