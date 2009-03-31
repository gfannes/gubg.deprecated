module gubg.file;

import tango.io.File;
import tango.io.FileSystem;

bool loadFile(out char[] content, in char[] fileName)
{
    File file = new File(fileName);
    content = cast(char[])file.read();
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
