module gubg.Timer;

import std.date;

enum ResetType {Auto, NoAuto}
class Timer
{
    this (ResetType resetType)
    {
        resetType_ = resetType;
        reset();
    }

    real difference()
    {
        auto diff = getUTCtime() - startTime_;
        if (ResetType.Auto == resetType_)
            reset();
        return diff/ticksPerSecond + cast(real)(diff%ticksPerSecond)/ticksPerSecond;
    }

    void reset() { startTime_ = getUTCtime(); }

    private:
    d_time startTime_;
    ResetType resetType_;
}

version (UnitTest)
{
    import core.thread;
    import std.stdio;
	void main()
	{
		auto timer = new Timer(ResetType.Auto);
        //Sleep for 11 miliseconds
		Thread.sleep(110000000);
		real vals[10];
		Thread.sleep(10000000);
		foreach (ref v; vals)
		{
			v = timer.difference;
			Thread.sleep(5000000);
		}
		foreach (v; vals)
            writefln("%10.20g", v);
	}
}
