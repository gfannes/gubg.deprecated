module gubg.file;

import tango.io.File;

bool loadFile(out char[] content, in char[] fileName)
{
    File file = new File(fileName);
    content = cast(char[])file.read();
    return true;
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
