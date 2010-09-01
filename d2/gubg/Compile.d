module gubg.Compile;

import std.format;
import std.process;
import std.range;
import std.stdio;

class CompileCommand
{
    this (string execFilepath)
    {
        execFilepath_ = execFilepath;
    }

    void addSource(string sourceFilepath)
    {
        sourceFilepaths_ ~= sourceFilepath;
    }

    bool execute()
    {
        //Create the command string
        auto command = appender!string();
        formattedWrite(command, "dmd -of%s", execFilepath_);
        foreach (sourceFilepath; sourceFilepaths_)
            formattedWrite(command, " %s", sourceFilepath);

        //Execute the command
        writefln("Command that will be executed:\n%s", command.data);
        if (0 != system(command.data))
            return false;

        return true;
    }

    private:
    string execFilepath_;
    string[] sourceFilepaths_;
}
