module Runtime;

import std.stdio;

bool verbose_;
void setVerbose(bool b)
{
    verbose_ = b;
    writefln("Verbose is set to %s", verbose_);
}
bool verbose(){return verbose_;}

string configurationFilename()
{
    version (Posix)
        return "gb.linux.json";
    version (Windows)
        return "gb.windows.json";
}
