module gubg.Vector;

import tango.math.Math;

real inprod(real[] lhs, real[] rhs)
{
    real inp = 0.0;
    if (lhs.length != rhs.length)
	throw new Exception("Length is different when computing the inproduct.");
    foreach (ix, v; lhs)
	inp += v * rhs[ix];
    return inp;
}

uint maxIndex(real[] values)
{
    uint res;
    real maxValue;
    foreach (ix, v; values)
	if (ix == 0 || v > maxValue)
	{
	    res = ix;
	    maxValue = v;
	}
    return res;
}

real sum(real[] values)
{
    real sum = 0.0;
    foreach (v; values)
	sum += v;
    return sum;
}

real mean(real[] values)
{
    return sum(values)/values.length;
}

void shiftMeanTo(real[] values, real wantedMean = 0.0)
{
    real diff = wantedMean - mean(values);
    foreach (inout v; values)
	v += diff;
}

void normalizeL1(real[] values)
{
    real sumAbs = 0.0;
    foreach (v; values)
	sumAbs += (v >= 0 ? v : -v);
    foreach (inout v; values)
        v /= sumAbs;
}
real l2Norm(real[] values)
{
    real res = 0.0;
    foreach (v; values)
	res += v*v;
    res = sqrt(res);
    return res;
}

void multiplyWith(real[] values, real factor)
{
    foreach (inout v; values)
	v *= factor;
}
void invertSign(real[] values)
{
    foreach (inout v; values)
	v = -v;
}

void add(real[] target, real[] source)
{
    foreach (ix, inout v; target)
	v += source[ix];
}
void add(real[] target, real[] source, real factor)
{
    foreach (ix, inout v; target)
	v += factor*source[ix];
}

real[] direction(real[] from, real[] to)
{
    real[] res = to.dup;
    foreach (ix, inout v; res)
	v -= from[ix];
    return res;
}

real distance(real[] from, real[] to)
{
    return l2Norm(direction(from, to));
}

version (UnitTest)
{
    import gubg.Puts;

    void main()
    {
	real[] a = [1, 2, 3];
	real[] b = [2, 1, 3];
	puts("inp = {}", a.inprod(b));
    }
}