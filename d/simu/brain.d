module simu.brain;

import tango.time.StopWatch;
import tango.core.Thread;
import tango.core.Memory;

import simu.region;
import gubg.puts;

class Brain
{
    bool add(Region region)
	{
	    mRegions ~= [region];
	    return true;
	}

    bool run(long totalTimeSec)
	{
	    mStopWatch.start();

	    ulong elapsedTime;
	    ulong prevTime;
	    while (totalTimeSec*1_000_000 > (elapsedTime = mStopWatch.microsec()))
	    {
//		puts("Elapsed time = {} sec", elapsedTime/1000000.0);
		foreach (region; mRegions)
		{
		    if (!region.update(mState, elapsedTime, elapsedTime-prevTime))
		    {
			puts("Updating region failed");
			return false;
		    }
		}
		prevTime = elapsedTime;
// 		puts("State = {}", mState);
// 		Thread.sleep(0.1);
	    }
	    return true;
	}
private:
    real[] mState;
    StopWatch mStopWatch;
    Region[] mRegions;
}

version (UnitTest)
{
    import simu.region;
    import gubg.visu;
    import gubg.hopfield;

    void main()
    {
	auto brain = new Brain();
	auto visu = new Visu(640, 480);
	auto spin = new Spin(2, 1.0, visu);
	auto nn = new NN(10, spin, visu);

	bool drawCallback()
	{
//	    puts("Elapsed time = {}", visu.elapsedTime()/1000000.0);
	    puts("Collecting");
	    GC.collect();
	    return true;
	}
	visu.show(true, &drawCallback);

	brain.add(spin);
	brain.add(nn);

	nn.setLearning(true);
	brain.run(20);
	nn.setLearning(false);
	brain.run(20);
	
	visu.stop();
    }
}