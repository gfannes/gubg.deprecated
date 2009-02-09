module rinle.rinle;

import gubg.puts;

import rinle.commander;
import rinle.globals;

import tango.core.Thread;


int main()
{
    putsFile("/tmp/rinle.log");

    puts("***Starting up rinle.\n\n");
    scope(exit)
    {
	puts("\n\n***Ending rinle.");
	if (env !is null)
	    delete env;
	env = null;
    }

    env = new Environment("./");

    bool finished = false;
    void quit(){finished = true;}

    scope commander = new NavigationCommander(&quit);

    while (!finished)
    {
	if (commander.waitForCommand)
	{
	    commander.executeCommand;
	} else
	{
	    env.ui.log("Could not understand command " ~ commander.commandString);
	    commander.reset;
	}
    }

    return 0;
}
