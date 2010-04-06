module gubg.Timer;

import tango.time.StopWatch;

class Timer
{
	this (bool restart = true)
		{
            restart_ = restart;
            start_();
		}
	
	real difference()
		{
			ulong diff;
			difference(diff);
			return 0.000001*diff;
		}

	void difference(inout ulong diff)
		{
			diff = stopwatch_.microsec();
            if (restart_)
                start_();
		}

	void difference(inout uint diff)
		{
			ulong d;
			difference(d);
			diff = d;
		}

private:
	void start_()
		{
            stopwatch_.start();
		}

	StopWatch stopwatch_;
    bool restart_;
}

version (UnitTest)
{
	import gubg.Puts;
	import tango.core.Thread;

	void main()
	{
		auto timer = new Timer;
		Thread.sleep(11);
		real vals[10];
		Thread.sleep(1);
		foreach (inout v; vals)
		{
			v = timer.difference;
			Thread.sleep(0.5);
		}
		foreach (v; vals)
			puts("{:e6}", v);
	}
}
