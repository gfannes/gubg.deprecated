module gubg.Array;

void setSame(T)(T[] values, T value)
{
    foreach (inout v; values)
	v = value;
}

//If nrElements == -1, the size of dst is taken
void spreadEqual(T, TT)(inout T[] dst, TT min, TT max, long nrElements = -1)
{
    if (nrElements == -1)
        nrElements = dst.length;
    if (dst.length != nrElements)
        dst.length = nrElements;
    switch (nrElements)
    {
        case 0:
            //Not much to do
            break;
        case 1:
            //Lets take the mean
            dst[0] = 0.5*(min+max);
            break;
        default:
            T d = cast(T)(max-min)/(nrElements-1);
            foreach (ix, inout v; dst)
                v = min + ix*d;
            break;
    }
}

T append(T)(inout T[] ary, T v)
{
    ary ~= [v];
    return v;
}

void assign(T)(inout T[] dst, inout T[] src)
{
	if (src)
	{
		dst.length = src.length;
		dst[] = src;
	} else
	{
		dst.length = 0;
	}
}

bool minMax(T)(inout T min, inout T max, T[] values)
{
    if (values.length == 0)
        return false;

    min = max = values[0];
    foreach (ix, v; values[1 .. $])
    {
        if (v < min)
            min = v;
        else if (v > max)
            max = v;
    }
    return true;
}
bool minMax(T)(T[2] mM, T[] values)
{
    return minMax(mM[0], mM[1], values);
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
