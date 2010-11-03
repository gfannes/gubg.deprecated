module gubg.OptionParser;

//OptionParser can be used to parse command-line options:
// > bool[string] switches;//Will reflect the options specified by the user
// > auto parser = new OptionParser;
// > switches["verbose"] = false;//Default setting for "verbose"
// > parser.addSwitch("-v", "--verbose", delegate void(){switches["verbose"] = true;});
// > writeln(parser.help);
// > parser.parse(args, true);//Parses the options and strips the executable

import gubg.Format;
import std.array;

alias void delegate() SwitchDelegate;
alias void delegate(string value = null) OptionalDelegate;
alias void delegate(string value) MandatoryDelegate;
class OptionParser
{
    this(string caption)
    {
        help_ = [caption];
    }
    void addSwitch(string shortHand, string longHand, string explanation, SwitchDelegate dg)
    {
        switchDelegates_[shortHand] = dg;
        switchDelegates_[longHand] = dg;
        addHelpLine_(shortHand, longHand, explanation);
    }
    void addMandatory(string shortHand, string longHand, string explanation, MandatoryDelegate dg)
    {
        mandatoryDelegates_[shortHand] = dg;
        mandatoryDelegates_[longHand] = dg;
        addHelpLine_(shortHand, longHand, explanation);
    }
    void addOptional(string shortHand, string longHand, string explanation, OptionalDelegate dg)
    {
        optionalDelegates_[shortHand] = dg;
        optionalDelegates_[longHand] = dg;
        addHelpLine_(shortHand, longHand, explanation);
    }
    void addLine(string line)
    {
        help_ ~= line;
    }

    string help()
    {
        Format format;
        format.delimiter = "\n";
        foreach (h; help_)
            format(h);
        return cast(string)format;
    }

    bool parse(ref string[] args, bool stripExe = true)
    {
        if (stripExe)
            args.popFront;

        //When args is empty, we are certainly done
        while (!args.empty)
        {
            if (args.front in switchDelegates_)
            {
                switchDelegates_[args.front]();
                args.popFront;
            } else if (args.front in mandatoryDelegates_)
            {
                if (args.length < 2)
                    return false;
                mandatoryDelegates_[args.front](args[1]);
                args.popFront;
                args.popFront;
            } else if (args.front in optionalDelegates_)
            {
                if (args.length < 2 || args[1].length == 0 || '-' == args[1][0])
                {
                    optionalDelegates_[args.front](null);
                } else
                {
                    optionalDelegates_[args.front](args[1]);
                    args.popFront;
                }
                args.popFront;
            } else
                break;
        }
        return true;
    }

    private:
    void addHelpLine_(string shortHand, string longHand, string explanation)
    {
        help_ ~= Format.immediate("\t%s\t%s\t%s", shortHand, longHand, explanation);
    }

    string[] help_;
    SwitchDelegate[string] switchDelegates_;
    MandatoryDelegate[string] mandatoryDelegates_;
    OptionalDelegate[string] optionalDelegates_;
}

version (UnitTest)
{
    import std.stdio;
    import c_process = std.c.process;
    void main(string[] args)
    {
        void printArgs(string msg)
        {
            writeln("\n", msg);
            foreach (arg; args)
                writeln(arg);
            writeln();
        }

        printArgs("Before");

        //Parse args
        bool[string] switches;
        string[string] values;
        {
            auto parser = new OptionParser("UnitTest for OptionParser");

            parser.addSwitch("-h", "--help", "Prints this help", delegate void(){writeln(parser.help);c_process.exit(0);});
            switches["verbose"] = false;
            parser.addSwitch("-v", "--verbose", "Switch on extra output", delegate void(){switches["verbose"] = true;});

            values["output"] = "";
            parser.addMandatory("-o", "--output <filename>", "Save output in <filename>", delegate void(string filename){values["output"] = filename;});
            values["error"] = "none";
            parser.addOptional("-e", "--error [filename]", "Show errors, save them in <filename> if given", delegate void(string error){values["error"] = (error is null ? "default" : error);});
            parser.addLine("\nWritten by Geert Fannes.");

            if (parser.parse(args))
                writeln("Parsing went OK");
            else
                writefln("Failed to parse %s", args.front);
        }

        printArgs("After");

        writefln("Verbose = %s", switches["verbose"]);
        writefln("Output = %s", values["output"]);
        writefln("Error = %s", values["error"]);
    }
}
