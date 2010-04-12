module gubg.Matrix;

import gubg.Vector;

interface IMatrix
{
    uint nrRows();
    uint nrCols();
    real[] opMul(real[] rhs);
    void multiply(real[] res, real[] rhs);
    real leftRightProduct(real[] vector);
    bool isSquare();
    real determinant();
}

class FullMatrix: IMatrix
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

    //IMatrix interface
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

        real res = 1.0;
        foreach (ix, row; rows_)
            res *= row[ix];
        return res;
    }

    private:
    real[][] rows_;
}

class DiagonalMatrix: IMatrix
{
    this(uint nr)
    {
        diagonal_.length = nr;
        foreach (inout v; diagonal_)
            v = 1.0;
    }

    static DiagonalMatrix createIdentity(uint nr)
    {
        DiagonalMatrix res = new DiagonalMatrix(nr);
        for (uint i = 0; i < nr; ++i)
            res.diagonal_[i] = 1.0;
        return res;
    }

    //IMatrix interface
    uint nrRows(){return diagonal_.length;}
    uint nrCols(){return diagonal_.length;}
    real[] opMul(real[] rhs)
    {
        real[] res = new real[nrRows];
        multiply(res, rhs);
        return res;
    }
    void multiply(real[] res, real[] rhs)
    {
        foreach (ix, d; diagonal_)
            res[ix] = d*rhs[ix];
    }
    real leftRightProduct(real[] vector)
    {
        real res = 0.0;
        foreach (ix, d; diagonal_)
            res += d*vector[ix]*vector[ix];
        return res;
    }
    bool isSquare(){return true;}
    real determinant()
    {
        real res = 1.0;
        foreach (d; diagonal_)
            res *= d;
        return res;
    }

    private:
    real[] diagonal_;
}

version (UnitTest)
{
    import gubg.Puts;

    void main()
    {
        auto m = new FullMatrix(3,5);
        auto id = DiagonalMatrix.createIdentity(5);
        real[] v = [1,2,3,4,5];
        puts("m.nrCols = {}", m.nrCols);
        puts("res = {}", m*v);
        puts("res = {}", id*v);
    }
}
