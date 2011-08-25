module Exit;

import std.stdio;
import c_process = std.c.process;

//Functionality to quit the program
enum ExitCode
{
    ok,
    error
}
void exitApp(T...)(ExitCode exitCode, string formatString, T args)
{
    switch (exitCode)
    {
        case ExitCode.error: write("ERROR::"); break;
        default: break;
    }
    writefln(formatString, args);
    final switch (exitCode)
    {
        case ExitCode.ok: c_process.exit(0); break;
        case ExitCode.error: c_process.exit(-1); break;
    }
}
