module gb;

import Commands;
import Exit;
import Runtime;
import gubg.Format;
import gubg.OptionParser;
import std.stdio;
import std.array;

immutable int versionMajor_ = 0;
immutable int versionMinor_ = 0;
immutable int versionBuild_ = 0;

//gb [options] <command> <location(s)>
//Options:
// * -v/--verbose
// * -h/--help
// * -V/--version
// * -o/--output <filename>
//Commands:
// * tags
// * exe
// * unit
// * lib
// * doc
// * clean
// * config
//Location(s):
// * Sourcefile
// * "gb.json" file
// * Directory where to look for a gb.json file
int main(string[] args)
{
    //We will collect all command-line options in these two hashes
    string[string] values;

    //The OptionParser we will use to parse the command-line arguments
    auto parser = new OptionParser("Usage: gb [switches/options] <command> <location(s)>");

    //Options:
    parser.addLine("\nOptions:");
    // -h/--help
    parser.addSwitch("-h", "--help", "Print this help", (){exitApp(ExitCode.ok, parser.help);}); 
    // -v/--verbose
    parser.addSwitch("-v", "--verbose", "Enable extra output", (){Runtime.setVerbose(true);});
    // -V/--version
    parser.addSwitch("-V", "--version", "Print the version", (){exitApp(ExitCode.ok, "Version %s.%s.%s", versionMajor_, versionMinor_, versionBuild_);});

    //Commands
    parser.addLine("\nCommands:");
    parser.addLine("\ttags\tCreate a ctags file");
    parser.addLine("\texe\tCreate an executable");
    parser.addLine("\tunit\tCreate and run a unit test");
    parser.addLine("\tlib\tCreate a library");
    parser.addLine("\tdoc\tCreate documentation");
    parser.addLine("\tclean\tClean the file cache");
    parser.addLine("\tconfig\tCreate a dummy config file");

    parser.addLine("\nWritten by Geert Fannes.");

    //Parse the options, they will get removed from args
    if (!parser.parse(args))
        exitApp(ExitCode.error, Format.immediate("Failed to parse option \"%s\"\n\n%s", args.front, parser.help));

    //Create the command from the left-over arguments
    auto command = Commands.createCommand(args);
    if (command is null)
    {
        if (args.empty)
            exitApp(ExitCode.error, Format.immediate("No command was specified.\n\n%s", parser.help));
        else
            exitApp(ExitCode.error, Format.immediate("Could not recognize the command from \"%s\".\n\n%s", args.front, parser.help));
    }

    //Execute the command
    if (!command.execute)
            exitApp(ExitCode.error, Format.immediate("Failed to execute %s.", command.toString));

    //Everything went OK, time to leave
    exitApp(ExitCode.ok, Format.immediate("Everything went OK."));

    return 0;
}
