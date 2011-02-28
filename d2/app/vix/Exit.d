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

scope class Scoper
{
    this (string msg)
    {
        msg_ = msg;
        writefln("Starting scope %s", msg_);
    }
    ~this ()
    {
        writefln("Leaving scope %s", msg_);
    }
    private:
    string msg_;
}

static this()
{
    signal(SIGINT, &catchInterrupt);
}
