module gubg.puts;

import tango.util.log.Trace;
import tango.text.convert.Layout;
private import tango.io.Console;
import tango.io.device.File;
import tango.io.model.IConduit;
import tango.io.Path;

static this ()
{
    _outputStream = Cout.stream;
    _deleteAtEnd = false;
}

static ~this ()
{
    _outputStream.close;
    if (_deleteAtEnd)
	FS.remove(_path);
}

OutputStream _outputStream;
char[] _path;
bool _deleteAtEnd;
public void putsFile(char[] path, bool deleteAtEnd = true)
{
    if (cast(File)_outputStream !is null)
	_outputStream.close;
    _outputStream = new File(path, File.ReadWriteCreate);
    _path = path.dup;
    _deleteAtEnd = deleteAtEnd;
}

// Write to stdout with trailing newline
public void puts(char[] fmt, ...)
{
    synchronized(Trace)
    {
	Layout!(char) layout = new Layout!(char);
	uint sink (char[] s)
	{
	    return _outputStream.write(s);
	}
	layout.convert (&sink, _arguments, _argptr, fmt);
	_outputStream.write("\n");
	_outputStream.flush;
    }
}
// Write to stdout without formatting and with trailing newline
public void putsNoFormat(char[] str)
{
    synchronized(Trace)
    {
	_outputStream.write(str);
	_outputStream.write("\n");
	_outputStream.flush;
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

scope class Log
{
    static this ()
	{
	    _level = 0;
	}
    this (char[] msg)
	{
	    _msg = msg.dup;
	    .puts(indent() ~ ">>> " ~ _msg);
	    ++_level;
	}
    ~this ()
	{
	    --_level;
	    .puts(indent() ~ "<<< " ~ _msg);
	}

    void puts(char[] fmt, ...)
	{
            synchronized(Trace)
            {
                _outputStream.write(indent());
                Layout!(char) layout = new Layout!(char);
                uint sink (char[] s)
                {
                    return _outputStream.write(s);
                }
                layout.convert (&sink, _arguments, _argptr, fmt);
                _outputStream.write("\n");
                _outputStream.flush;
            }
//	    .puts(indent() ~ msg);
	}
    void putsNoFormat(char[] str)
	{
            synchronized(Trace)
            {
                _outputStream.write(indent());
		_outputStream.write(str);
                _outputStream.write("\n");
                _outputStream.flush;
            }
//	    .puts(indent() ~ msg);
	}
private:
    char[] _msg;
    static uint _level;
    char[] indent()
	{
	    char[] id;
	    id.length = _level*2;
	    foreach (inout ch; id)
		ch = ' ';
	    return id;
	}
}

version (UnitTest)
{
    void main()
    {
	puts("asdfasdf");
	puts("asdfasdf = {}","asas");
	putsFile("/tmp/test.txt");
	puts("hallo beste kerel");
    }
}
