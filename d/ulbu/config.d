module ulbu.config;

// Parses a config file
import gubg.file;
import gubg.parser.tokenSequence;
import gubg.puts;

import ulbu.language;
import ulbu.create;

// For the Create mixin
public uint level = 0;
alias TokenSequence!(ConfigLanguage) CTS;

class Config
{
    this (char[] location)
        {
            mLocation = location;
        }

    char[] location(){return mLocation;}

    mixin Create!(CTS);
    static void createI(inout Config res, inout CTS ts)
	{
            if (ts.isKeyword("location") && ts.isSymbol("="))
            {
                char[] location;
                if (ts.isSymbol("."))
                {
                    puts("{}Found the root", indent);
                    location = ".";
                } else
                    throw new Exception("Not finished yet.");
                res = new Config(location);
            }
	}

    static Config createFrom(char[] dirName, char[] fileName = "ulbu.config")
        {
            // Load the source file
            char[] sourceCode;
            loadFile(sourceCode, dirName ~ "/" ~ fileName);
            
            // Create its token sequence
            auto ts = new CTS(sourceCode);

            // Create the module based on the token sequence
            return Config.create(ts);
        }

private:
    char[] mLocation;
}

version (Test)
{
    void main()
    {
        auto config = Config.createFrom("test");
    }
}
