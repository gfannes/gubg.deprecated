module gubg.random;

import tango.math.random.Kiss;

real uniform()
{
    return Kiss.instance.fraction;
}

real uniform(real min, real max)
{
    return min + uniform()*(max-min);
}

real[] uniform(real min, real max, uint nr)
{
    real[] res;
    res.length = nr;
    foreach (inout v; res)
	v = uniform(min, max);
    return res;
}

version (UnitTest)
{
    import gubg.puts;

    void main()
    {
	auto ary = uniform(0.0, 1.0, 10);
	puts("ary = {}", ary);
    }
}
