module vix.vix;

import vix.Exit;
import vix.Controller;

import gubg.Profiler;
import gubg.Timer;
import gubg.OptionParser;

import std.stdio;

bool verbose__ = false;
int main(string[] args)
{
    bool[string] switches;
    auto parser = new OptionParser("vix, the vi file explorer");
    //Default settings
    switches["verbose"] = false;
    parser.addSwitch("-v", "--verbose", "Verbosity", delegate void(){switches["verbose"] = true;});
    switches["small"] = false;
    parser.addSwitch("-s", "--small", "Start vix in small-mode", delegate void(){switches["small"] = true;});
    //Parses the options and strips the executable
    parser.parse(args, true);

    //Profiling variables
    auto p = new Profiler("Mainloop");
    uint nrFrames = 0;
    auto timer = Timer(ResetType.NoAuto);

    version (Posix) uint width = 1920, height = 1050;
    version (Win32) uint width = 1440, height = 850;
    if (switches["small"])
    {
        width = 640;
        height = 480;
    }
    auto controller = new Controller(width, height);

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
