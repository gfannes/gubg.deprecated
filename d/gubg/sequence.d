module gubg.sequence;

import gubg.math;
import gubg.puts;

import tango.math.Math;

// model: log(p_i*cte) = x_i
// cte = (p_0*...*p_n)^(1/n) => sum_j(x_j) = 0
// p_i = exp(x_i)/sum_j(exp(x_j))
class Sequence
{
    this (real stay, real move, real reset, real[] probs)
	{
	    _stay = stay;
	    _move = move;
	    _reset = reset;
	    _probs = probs;
	    _currentValues = new real[nrStates];
	    foreach (inout v; _currentValues)
		v = 0.0;
	}
    
    real[] probs(){return _probs;}

    real[] update(real dT)
	{
	    auto res = targetValues;
	    foreach (ix, inout p; res)
		p = exp(_currentValues[ix] + dT*(p - _currentValues[ix]));
	    auto invSum = 1.0/sum(res);
	    foreach (inout p; res)
		p *= invSum;
	    return res;
	}

    real[] targetValues()
	{
	    real[] res = new real[nrStates];
	    foreach (ix, inout v; res)
	    {
		res[ix] = _stay*_probs[ix];
		if (ix == 0)
		    res[ix] += _reset;
		else
		    res[ix] += _move*_probs[ix-1];
	    }
	    normalize(res);
	    return res;
	}

    uint nrStates(){return _probs.length;}

private:
    real _stay;
    real _move;
    real _reset;
    rea[] _probs;
    real[] _currentValues;
}

version (UnitTest)
{
    void main()
    {
	real[] values = [10, 11, 9];
	auto seq = new Sequence(values);
	puts("probs = {}, sum = {}", seq.probs, sum(seq.probs));
	puts("values = {}", values);
    }
}
