module gubg.sequence;

import gubg.math;
import gubg.vector;
import gubg.puts;

import tango.math.Math;

// model: log(p_i*cte) = x_i
// cte = (p_0*...*p_n)^(1/n) => sum_j(x_j) = 0
// p_i = exp(x_i)/sum_j(exp(x_j))

// q(dT) = prob to stay in state during dT timespan
// q(dT) = q(1)^dT
// j(dT) = prob to jump to state j in timespan dT
// j(dT) = j(1)*(1-q(dT))/(1-q(1))
class Sequence
{
    this (real stay, real move, real reset, real[] probs)
	{
            auto sum = stay+move+reset;
	    _stay = stay/sum;
	    _move = move/sum;
	    _reset = reset/sum;

	    move *= 0.2;
	    sum = stay+move+reset;
	    _mixStay = stay/sum;
	    _mixMove = move/sum;
	    _mixReset = reset/sum;

	    move *= 0.2;
	    sum = stay+move+reset;
	    _nmixStay = stay/sum;
	    _nmixMove = move/sum;
	    _nmixReset = reset/sum;

	    _probs = probs;
            normalizeL1(_probs);
	}
    
    real[] probs(){return _probs;}

    void update(real dT)
	{
            auto oldProbs = new real[nrStates];
            scope(exit) delete oldProbs;
            oldProbs[] = _probs[];
            real cachedStayProb = pow(_stay, dT);
            real cachedMoveProb = _move*(1.0-cachedStayProb)/(1.0-_stay);
            real cachedResetProb = _reset*(1.0-cachedStayProb)/(1.0-_stay);
	    uint mix = maxIndex(oldProbs);
	    _probs.setSame(0.0);
            foreach (ix, inout p; oldProbs)
	    {
		real stayProb = cachedStayProb, moveProb = cachedMoveProb, resetProb = cachedResetProb;
		if (ix == mix)
		{
		    stayProb = pow(_mixStay, dT);
		    moveProb = _mixMove*(1.0-stayProb)/(1.0-_mixStay);
		    resetProb = _mixReset*(1.0-stayProb)/(1.0-_mixStay);
		} else if (ix == (mix+1)%nrStates)
		{
		    stayProb = pow(_nmixStay, dT);
		    moveProb = _nmixMove*(1.0-stayProb)/(1.0-_nmixStay);
		    resetProb = _nmixReset*(1.0-stayProb)/(1.0-_nmixStay);
		}
		_probs[0] += p * resetProb;
		_probs[ix] += p * stayProb;
		_probs[(ix+1)%$] += p * moveProb;
	    }
            normalizeL1(_probs);
	}

    uint nrStates(){return _probs.length;}

private:
    real _stay;
    real _move;
    real _reset;
    real _mixStay;
    real _mixMove;
    real _mixReset;
    real _nmixStay;
    real _nmixMove;
    real _nmixReset;
    real[] _probs;
    real[] _currentValues;
}

version (UnitTest)
{
    import gubg.timer;
    import tango.core.Thread;
    void main()
    {
        real stay = 0.001, move = 100.0, reset = 0.0;
        real[] probs = [1.0, 0.0, 0.0];
	auto seq = new Sequence(stay, move, reset, probs);
	puts("probs = {}, sum = {}", seq.probs, sum(seq.probs));
        
        auto timer = new Timer;
        for (uint i = 0; i < 10000; ++i)
        {
	    real dT = timer.difference;
            seq.update(dT);
            auto p = seq.probs;
            puts("probs = {:g9}, {:g9}, {:g9}, ix = {}, dT = {:e9}", p[0], p[1], p[2], maxIndex(seq.probs), dT);
            Thread.sleep(0.01);
        }
    }
}

version (UnitTest2)
{

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
