module gubg.puts;

import tango.util.log.Trace;
import tango.text.convert.Layout;
private import tango.io.Console;
import tango.io.stream.FileStream;
import tango.io.model.IConduit;

static this ()
{
    mOutputStream = Cout.stream;
}

static ~this ()
{
    mOutputStream.close;
}

OutputStream mOutputStream;
public void putsFile(char[] path)
{
    if (cast(FileOutput)mOutputStream !is null)
	mOutputStream.close;
    mOutputStream = new FileOutput(path);
}

// Write to stdout with trailing newline
public void puts(char[] fmt, ...)
{
    synchronized(Trace)
    {
	Layout!(char) layout = new Layout!(char);
	uint sink (char[] s)
	{
	    return mOutputStream.write(s);
	}
	layout.convert (&sink, _arguments, _argptr, fmt);
	mOutputStream.write("\n");
	mOutputStream.flush;
    }
}

// Write to stdout without trailing newline
public void putsn(char[] fmt, ...)
{
    synchronized(Trace)
    {
	Layout!(char) layout = new Layout!(char);
	uint sink (char[] s)
	{
	    return Cout.stream.write(s);
	}
	layout.convert (&sink, _arguments, _argptr, fmt);
	Cout.flush;
    }
}

// Write to stderr with trailing newline
public void err(char[] fmt, ...)
{
    synchronized(Trace)
    {
	Cerr.stream.write("ERROR::");
	Layout!(char) layout = new Layout!(char);
	uint sink (char[] s)
	{
	    return Cerr.stream.write(s);
	}
	layout.convert (&sink, _arguments, _argptr, fmt);
	Cerr.stream.write("\n");
	Cerr.flush;
    }
}

// Write to stderr without trailing newline
public void errn(char[] fmt, ...)
{
    synchronized(Trace)
    {
	Cerr.stream.write("ERROR::");
	Layout!(char) layout = new Layout!(char);
	uint sink (char[] s)
	{
	    return Cerr.stream.write(s);
	}
	layout.convert (&sink, _arguments, _argptr, fmt);
	Cerr.flush;
    }
}

version (Test)
{
    void main()
    {
	puts("asdfasdf");
	puts("asdfasdf = {}","asas");
	putsFile("/tmp/test.txt");
	puts("hallo beste kerel");
    }
}
