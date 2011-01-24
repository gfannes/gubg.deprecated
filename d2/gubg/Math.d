module gubg.Math;

//T(from) = linTrans[0]*from + linTrans[1] == to
void computeLinTrans(T, TT, TTT)(ref T[2] linTrans, TT[2] from, TTT[2] to)
{
    linTrans[0] = (to[1]-to[0])/(from[1]-from[0]);
    linTrans[1] = to[0] - linTrans[0]*from[0];
}
T transformLinTrans(T, TT)(TT from, T[2] linTrans)
{
    return linTrans[0]*from + linTrans[1];
}
T transformReverseLinTrans(T, TT)(TT to, T[2] linTrans)
{
    return (to - linTrans[1])/linTrans[0];
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        real[2] lt;

        computeLinTrans!(real, uint, uint)(lt, [0, 1], [0, 1]);
        writefln("lt: %s", lt);
    }
}
