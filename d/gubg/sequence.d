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

	    move *= 20.0;
	    sum = stay+move+reset;
	    _mixStay = stay/sum;
	    _mixMove = move/sum;
	    _mixReset = reset/sum;

	    _probs = probs;
            normalizeL1(_probs);
	}
    
    real[] probs(){return _probs;}

    void update(real dT)
	{
            auto oldProbs = new real[nrStates];
            scope(exit) delete oldProbs;
            oldProbs[] = _probs[];
            real stayProb = pow(_stay, dT);
            real moveProb = _move*(1.0-stayProb)/(1.0-_stay);
            real resetProb = _reset*(1.0-stayProb)/(1.0-_stay);
            real mixStayProb = pow(_mixStay, dT);
            real mixMoveProb = _mixMove*(1.0-mixStayProb)/(1.0-_mixStay);
            real mixResetProb = _mixReset*(1.0-mixStayProb)/(1.0-_mixStay);
	    uint mix = maxIndex(oldProbs);
            foreach (ix, inout p; _probs)
                if (ix == 0)
                    p = oldProbs[ix]*(mix == 0 ? mixStayProb : stayProb) +
                        oldProbs[$-1]*(mix == nrStates-1 ? mixMoveProb : moveProb) + 
                        oldProbs[mix]*mixResetProb + (1.0 - oldProbs[mix])*resetProb;                        
                else
                    p = oldProbs[ix]*(mix == ix ? mixStayProb : stayProb) +
                        oldProbs[ix-1]*(mix == ix-1 ? mixMoveProb : moveProb);
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
    real[] _probs;
    real[] _currentValues;
}

version (UnitTest)
{
    import gubg.timer;
    import tango.core.Thread;
    void main()
    {
        real stay = 10.0, move = 0.2, reset = 0.0;
        real[] probs = [1.0, 0.0, 0.0];
	auto seq = new Sequence(stay, move, reset, probs);
	puts("probs = {}, sum = {}", seq.probs, sum(seq.probs));
        
        auto timer = new Timer;
        for (uint i = 0; i < 1000; ++i)
        {
            seq.update(timer.difference);
            auto p = seq.probs;
            puts("probs = {:g9}, {:g9}, {:g9}, ix = {}", p[0], p[1], p[2], maxIndex(seq.probs));
            Thread.sleep(0.1);
        }
    }
}

version (UnitTest2)
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
