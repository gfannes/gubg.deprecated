module gubg.vector;

real inprod(real[] lhs, real[] rhs)
{
    real inp = 0.0;
    if (lhs.length != rhs.length)
	throw new Exception("Length is different when computing the inproduct.");
    foreach (ix, v; lhs)
	inp += v * rhs[ix];
    return inp;
}

version (UnitTest)
{
    import gubg.puts;

    void main()
    {
	real[] a = [1, 2, 3];
	real[] b = [2, 1, 3];
	puts("inp = {}", a.inprod(b));
    }
}
