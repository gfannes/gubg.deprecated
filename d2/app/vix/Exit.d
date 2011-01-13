module vix.Exit;

import core.stdc.signal;
import core.stdc.stdlib;
import std.stdio;

void catchInterrupt(int v)
{
    writeln("Catched interrupt");
    exit(0);
}

static this()
{
    signal(SIGINT, &catchInterrupt);
}
