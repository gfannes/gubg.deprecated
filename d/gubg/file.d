module gubg.file;

import tango.io.device.File;
import tango.io.FileSystem;

bool loadFile(out char[] content, in char[] fileName)
{
    content = cast(char[])File.get(fileName);
    return true;
}

scope class ChangeDir
{
    this (char[] dirName)
        {
            mOrigDir = FileSystem.getDirectory();
            FileSystem.setDirectory(dirName);
        }

    ~this ()
        {
            FileSystem.setDirectory(mOrigDir);
        }
private:
    char[] mOrigDir;
}

version (UnitTest)
{
    import gubg.puts;

    void main()
    {
        char[] str;
        loadFile(str, "file.d");
	puts(str);
    }
}
