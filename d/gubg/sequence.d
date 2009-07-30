module gubg.sequence;

import gubg.math;
import gubg.vector;
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
		    res[ix] += _reset + _move*_probs[$-1];
		else
		    res[ix] += _move*_probs[ix-1];
	    }

	    auto maxIX = maxIndex(_probs);
	    puts("\nmaxIX = {}", maxIX);
	    puts("_probs = {}", _probs);
	    puts("targetValues before extra = {}", res);
	    res[(maxIX+1)%$] += 0.8*_move*_probs[maxIX];
//	    res[maxIX] -= 0.8*_stay*_probs[maxIX];

	    puts("targetValues before shift = {}", res);
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

version (UnitTest2)
{
    import gubg.timer;
    import tango.core.Thread;
    void main()
    {
        real stay = 1.0, move = 0.2, reset = 0.1;
        real[] probs = [0.1, 0.2, 0.3];
	auto seq = new Sequence(stay, move, reset, probs);
	puts("probs = {}, sum = {}", seq.probs, sum(seq.probs));
        
        auto timer = new Timer;
        for (uint i = 0; i < 100; ++i)
        {
            seq.update(timer.difference);
            auto p = seq.probs;
//            puts("probs = {:g9}, {:g9}, {:g9}, sum = {}", p[0], p[1], p[2], sum(seq.probs));
//            Thread.sleep(0.1);
        }
    }
}

version (UnitTest)
{
    import gubg.timer;
    import tango.core.Thread;

    class NewtonSpring
    {
	this(real springK)
	    {
		_springK = springK;
		_pos = _speed = _accel = 0.0;
		_externalAccel = 0.0;
	    }

	void setAccel(real accel){_externalAccel = accel;}
	void setSpeed(real speed){_speed = speed;}

	void update(real dT)
	    {
		_accel = _externalAccel + springAccel;
		_speed += dT*_accel;
		_pos += dT*_speed;
	    }

	real position(){return _pos;}
	real speed(){return _speed;}

    private:
	real springAccel(){return -_springK*_pos;}

	// Fixed
	real _springK;

	// External
	real _externalAccel;

	// Volatile
	real _pos;
	real _speed;
	real _accel;
    }

    void main()
    {
	auto timer = new Timer;
	auto m = new NewtonSpring(1);

	m.setSpeed(1.0);
	
	for (uint i = 0; i < 1000; ++i)
	{
	    Thread.sleep(0.1);
	    m.update(timer.difference);
	    puts("pos = {}", m.position);
	}
    }
}
