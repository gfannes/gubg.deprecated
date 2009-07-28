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
            normalizeL1(_probs);
	    _currentValues = new real[nrStates];
	    foreach (inout v; _currentValues)
		v = 0.0;
	}
    
    real[] probs(){return _probs;}

    void update(real dT)
	{
	    auto newProbs = targetValues;
	    foreach (ix, inout currentValue; _currentValues)
            {
                currentValue += dT*(newProbs[ix] - currentValue);
		newProbs[ix] = exp(currentValue);
            }
            normalizeL1(newProbs);
            _probs = newProbs;
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
	    res.shiftMeanTo(0.0);
	    return res;
	}

    uint nrStates(){return _probs.length;}

private:
    real _stay;
    real _move;
    real _reset;
    real[] _probs;
    real[] _currentValues;
}

version (UnitTest)
{
    import gubg.timer;
    import tango.core.Thread;
    void main()
    {
        real stay = 1.0, move = 0.2, reset = 2;
        real[] probs = [0.1, 0.2, 0.3];
	auto seq = new Sequence(stay, move, reset, probs);
	puts("probs = {}, sum = {}", seq.probs, sum(seq.probs));
        
        auto timer = new Timer;
        for (uint i = 0; i < 100; ++i)
        {
            seq.update(timer.difference);
            auto p = seq.probs;
            puts("probs = {:g9}, {:g9}, {:g9}, sum = {}", p[0], p[1], p[2], sum(seq.probs));
            Thread.sleep(0.1);
        }
    }
}
