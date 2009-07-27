module gubg.matrix;

import gubg.vector;

class Matrix
{
    this (uint nrRows, uint nrCols)
	{
	    _rows.length = nrRows;
	    foreach (inout row; _rows)
	    {
		row.length = nrCols;
		foreach (inout v; row)
		    v = 0.0;
	    }
	}

    uint nrRows(){return _rows.length;}
    uint nrCols(){return _rows[0].length;}

    real[] opMul(real[] rhs)
	{
	    real[] res = new real[nrRows];
	    foreach (ix, row; _rows)
		res[ix] = row.inprod(rhs);
	    return res;
	}

private:
    real[][] _rows;
}

version (UnitTest)
{
    import gubg.puts;

    void main()
    {
	auto m = new Matrix(3,5);
	real[] v = [1,2,3,4,5];
	puts("m.nrCols = {}", m.nrCols);
	puts("res = {}", m*v);
    }
}
