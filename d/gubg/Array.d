module gubg.Array;

void setSame(T)(T[] values, T value)
{
    foreach (inout v; values)
	v = value;
}

T append(T)(inout T[] ary, T v)
{
    ary ~= [v];
    return v;
}

version(UnitTest)
{
    import gubg.Puts;

    void main()
    {
	real[2] array;
	setSame!(real)(array, 0.0);
	puts("array = {}", array);
    }
}