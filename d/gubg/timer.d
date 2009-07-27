module gubg.timer;

import tango.time.Time;
import tango.time.Clock;

class Timer
{
    this ()
	{
	    start();
	}

    void start()
	{
	    _previous = getCurrentValue;
	    _startTime = Clock.now;
	    _startTickCount = getCurrentValue;
	    _synchronized = false;
	}

    real difference()
	{
	    if (!_synchronized)
	    {
		long micros = (Clock.now-_startTime).micros;
		if (micros <= 0)
		    micros = 1;
		_secondsPerTick = micros;
		_secondsPerTick /= (getCurrentValue - _startTickCount)*1000000;
		_synchronized = (micros > 10000000);
	    }
	    ulong diff;
	    difference(diff);
	    return _secondsPerTick;
	}

    void difference(inout ulong diff)
	{
	    ulong current = getCurrentValue;
	    diff = current - _previous;
	    _previous = current;
	}

    void difference(inout uint diff)
	{
	    ulong d;
	    difference(d);
	    diff = d;
	}

private:
    ulong getCurrentValue()
	{
	    uint high, low;
	    asm
	    {
		rdtsc;
		mov low, EAX;
		mov high, EDX;
	    }
	    ulong ret = high;
	    ret <<= 32;
	    ret |= low;
	    return ret;
	}

    ulong _previous;
    ulong _startTickCount;
    Time _startTime;
    bool _synchronized;
    real _secondsPerTick;
}

version (UnitTest)
{
import gubg.puts;
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
//	    Thread.sleep(0.5);
	}
	foreach (v; vals)
	    puts("{:e6}", v);
	return 0;
    }
}
