module gubg.math;

import tango.math.Math;

real roundCyclic(real x, real cyclic)
{
    real cyclic2 = 0.5*cyclic;
    return abs((x + cyclic2) % cyclic) - cyclic2;
}

real l2Dist(real[] lhs, real[] rhs)
{
    if (lhs.length == 2 && rhs.length == 2)
	return hypot(lhs[0]-rhs[0], lhs[1]-rhs[1]);
    else
    {
	real ss = 0.0;
	foreach (ix, l; lhs)
	    ss += (l-rhs[ix])*(l-rhs[ix]);
	return sqrt(ss);
    }
}

real sigmoid(real x)
{
    return 1.0/(1.0 + exp(-x));
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
