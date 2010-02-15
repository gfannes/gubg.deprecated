module gubg.File;

import tango.io.device.File;
import tango.sys.Environment;

bool loadFile(out char[] content, in char[] fileName)
{
    content = cast(char[])File.get(fileName);
    return true;
}

void exportLines(char[][] lines, char[] fileName)
{
    scope fo = new File(fileName, File.WriteCreate);	
    foreach (line; lines)
    {
        fo.write(line);
        fo.write("\n");
    }
}

scope class ChangeDir
{
    this (char[] dirName)
    {
        origDir_ = Environment.cwd();
        Environment.cwd(dirName);
    }

    ~this ()
    {
        Environment.cwd(origDir_);
    }
    private:
    char[] origDir_;
}

version (UnitTest)
{
    import gubg.Puts;

    void main()
    {
        char[] str;
        loadFile(str, "file.d");
        puts(str);
    }
}
