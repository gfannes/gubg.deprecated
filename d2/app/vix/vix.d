module vix.vix;

import vix.Exit;
import vix.Controller;

import gubg.Profiler;
import gubg.Timer;

import std.stdio;

bool verbose__ = true;
int main(string[] args)
{
    //Profiling variables
    auto p = new Profiler("Mainloop");
    uint nrFrames = 0;
    auto timer = Timer(ResetType.NoAuto);

    auto controller = new Controller;

    while (!controller.quit)
    {
        ++nrFrames;

        controller.process();

        //Every now and then, we write out profiling information
        const NrSeconds = 1.0;
        if (timer.difference > NrSeconds)
        {
            if (verbose__)
            {
                writeln(p.toString);
                writefln("FPS: %s", cast(real)nrFrames/NrSeconds);
            }
            timer.reset;
            p.reset();
            nrFrames = 0;
        }
    }
    return 0;
}
