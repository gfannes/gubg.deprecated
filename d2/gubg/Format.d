module gubg.Format;

import std.format;
import std.array;

//Allows for easy formatting of strings
struct Format
{
    string delimiter;

    //Static formatter that is easy to use if only one formatted string needs to be added to the format
    static string immediate(T...)(string formatString, T args)
    {
        scope Format format;
        format(formatString, args);
        return cast(string)format;
    }

    //Don't try to add something like this, it won't work:
    //Format format;
    //format("abc"); => this will now call the constructor and create a fresh Format struct with "abc" in it iso opCall.
    //this(T...)(string formatString, T args)
    //{
    //    allocateIfNecessary_();
    //    formattedWrite(appender_, formatString, args);
    //}
    void opCall(T...)(string formatString, T args)
    {
        //Write the delimiter before we allocate our appender_, making sure we don't prepend the delimiter before our first write
        writeDelimiter_();
        allocateIfNecessary_();
        formattedWrite(appender_, formatString, args);
    }
    string opCast(T)() if (is(T == string))
    {
        return appender_.data;
    }
    string toString()
    {
        return cast(string)this;
    }

    private:
    void writeDelimiter_()
    {
        if (delimiter && allocated_)
            formattedWrite(appender_, delimiter);
    }
    void allocateIfNecessary_()
    {
        if (!allocated_)
        {
            allocated_ = true;
            appender_ = appender!(string)();
        }
    }

    bool allocated_;
    Appender!(string) appender_;
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        Format format;
        format.delimiter = "\t";
        format("abc = %d", 42);
        format("def = %s", "asdfasf");
        format.delimiter = "\n";
        format("brol");
        format("brol");
        format("brol");
        writeln(cast(string)format);
    }
}
