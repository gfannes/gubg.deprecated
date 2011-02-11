module vix.Exit;

import core.stdc.signal;
import core.stdc.stdlib;
import std.stdio;

void catchInterrupt(int v)
{
    writeln("Catched interrupt");
    exit(0);
}

void reportError(string message)
{
    writefln("ERROR::%s", message);
}

static this()
{
    signal(SIGINT, &catchInterrupt);
}
