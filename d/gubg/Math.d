module gubg.Math;

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

//T(from) = linTrans[0]*from + linTrans[1] == to
void computeLinTrans(T, TT, TTT)(T[2] linTrans, TT[2] from, TTT[2] to)
{
    linTrans[0] = (to[1]-to[0])/(from[1]-from[0]);
    linTrans[1] = to[0] - linTrans[0]*from[0];
}
T transformLinTrans(T, TT)(TT from, T[2] linTrans)
{
    return linTrans[0]*from + linTrans[1];
}
