module gubg.test2;

import tango.core.Thread;

import gubg.log;

void printLoop()
{
    int i=0;
    while (true)
    {
	++i;
	log("A line of text.");
    }
}

void main()
{
    const int nrThreads = 10;

    // Create and start the threads
    Thread[] mThread;
    for (int i=0; i<nrThreads; ++i)
    {
	mThread ~= [new Thread(&printLoop)];
	mThread[i].start();
    }

    thread_joinAll();
}
