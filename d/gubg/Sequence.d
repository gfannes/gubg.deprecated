module gubg.Sequence;

import gubg.Math;
import gubg.Vector;
import gubg.Array;
import gubg.Puts;

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
	    stay_ = stay/sum;
	    move_ = move/sum;
	    reset_ = reset/sum;

	    move *= 0.2;
	    sum = stay+move+reset;
	    mixStay_ = stay/sum;
	    mixMove_ = move/sum;
	    mixReset_ = reset/sum;

	    move *= 0.2;
	    sum = stay+move+reset;
	    nmixStay_ = stay/sum;
	    nmixMove_ = move/sum;
	    nmixReset_ = reset/sum;

	    probs_ = probs;
            normalizeL1(probs_);
	}
    
    real[] probs(){return probs_;}

    void update(real dT)
	{
            auto oldProbs = new real[nrStates];
            scope(exit) delete oldProbs;
            oldProbs[] = probs_[];
            real cachedStayProb = pow(stay_, dT);
            real cachedMoveProb = move_*(1.0-cachedStayProb)/(1.0-stay_);
            real cachedResetProb = reset_*(1.0-cachedStayProb)/(1.0-stay_);
	    uint mix = maxIndex(oldProbs);
	    setSame!(real)(probs_, 0.0);
            foreach (ix, inout p; oldProbs)
	    {
		real stayProb = cachedStayProb, moveProb = cachedMoveProb, resetProb = cachedResetProb;
		if (ix == mix)
		{
		    stayProb = pow(mixStay_, dT);
		    moveProb = mixMove_*(1.0-stayProb)/(1.0-mixStay_);
		    resetProb = mixReset_*(1.0-stayProb)/(1.0-mixStay_);
		} else if (ix == (mix+1)%nrStates)
		{
		    stayProb = pow(nmixStay_, dT);
		    moveProb = nmixMove_*(1.0-stayProb)/(1.0-nmixStay_);
		    resetProb = nmixReset_*(1.0-stayProb)/(1.0-nmixStay_);
		}
		probs_[0] += p * resetProb;
		probs_[ix] += p * stayProb;
		probs_[(ix+1)%$] += p * moveProb;
	    }
            normalizeL1(probs_);
	}

    uint nrStates(){return probs_.length;}

private:
    real stay_;
    real move_;
    real reset_;
    real mixStay_;
    real mixMove_;
    real mixReset_;
    real nmixStay_;
    real nmixMove_;
    real nmixReset_;
    real[] probs_;
}

version (UnitTest)
{
    import gubg.Timer;
    import gubg.graphics.Visu;
    import tango.core.Thread;

    void main()
    {
        real stay = 0.1, move = 10.0, reset = 0.0;
	const NrStates = 10;
        real[] probs = new real[NrStates];
	probs[] = 1.0;
// 	probs[0] = 1.0;
// 	probs[NrStates/2] = 1.0;
	auto seq = new Sequence(stay, move, reset, probs);
	auto visu = new Visu(640, 480, [0,0], [640.0/NrStates, 480.0*NrStates/5]);
	auto of = new Factory(null, null);	// Factory used for creating squares and circles, we use no predefined stroke nor fill style
	of.fillColor(Color(0.5, 0.5, 0.5));
	puts("probs = {}, sum = {}", seq.probs, sum(seq.probs));

	// Start the visualization thread
	visu.show(true, null, 0.01);
	Rectangle[NrStates] rectanlges;
	foreach (ix, prob; seq.probs)
		visu.add(rectanlges[ix] = of.createRectangle([0.0+ix,0], [1.0+ix, prob]));
        
        auto timer = new Timer;
        for (uint i = 0; i < 10000; ++i)
        {
	    real dT = timer.difference;
            seq.update(dT);
            auto p = seq.probs;
//            puts("probs = {:g9}, {:g9}, {:g9}, ix = {}, dT = {:e9}", p[0], p[1], p[2], maxIndex(seq.probs), dT);
	    foreach (ix, prob; p)
		    rectanlges[ix].setCoordinates([0.0+ix,0], [1.0+ix, prob]);
            Thread.sleep(0.01);
        }
	visu.stop();
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
