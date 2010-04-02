module gubg.Matrix;

import gubg.Vector;

class Matrix
{
    this (uint nrRows, uint nrCols)
    {
        rows_.length = nrRows;
        foreach (inout row; rows_)
        {
            row.length = nrCols;
            foreach (inout v; row)
                v = 0.0;
        }
    }
    static Matrix identity(uint nr)
    {
        Matrix res = new Matrix(nr, nr);
        for (uint i = 0; i < nr; ++i)
            res.rows_[i][i] = 1.0;
        return res;
    }

    uint nrRows(){return rows_.length;}
    uint nrCols(){return rows_[0].length;}

    real[] opMul(real[] rhs)
    {
        real[] res = new real[nrRows];
        multiply(res, rhs);
        return res;
    }
    void multiply(real[] res, real[] rhs)
    {
        foreach (ix, row; rows_)
            res[ix] = row.inprod(rhs);
    }

    real leftRightProduct(real[] vector)
    {
        real res = 0.0;
        scope tmp = new real[vector.length];
        multiply(tmp, vector);
        return tmp.inprod(vector);

    }

    bool isSquare()
    {
        return nrRows == nrCols;
    }

    real determinant()
    {
        if (!isSquare)
            throw new Exception("The determinant can only be computed for square matrices.");
        //We only support diagonal matrices for now
        foreach (ix, row; rows_)
            foreach (ix2, v; row)
                if (v != (ix == ix2 ? 1.0 : 0.0))
                    throw new Exception("Only diagonal matrices are supported for determinant calculation.");

        real res = 0.0;
        foreach (ix, row; rows_)
            res += row[ix];
        return res;
    }

    private:
    real[][] rows_;
}

version (UnitTest)
{
    import gubg.Puts;

    void main()
    {
        auto m = new Matrix(3,5);
        real[] v = [1,2,3,4,5];
        puts("m.nrCols = {}", m.nrCols);
        puts("res = {}", m*v);
    }
}
