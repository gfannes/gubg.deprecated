module gubg.Matrix;

import gubg.Vector;

interface IMatrix
{
    uint nrRows();
    uint nrCols();
    real[] opMul(real[] rhs);
    void multiply(real[] res, real[] rhs);
    void multiply(real value);
    real leftRightProduct(real[] vector);
    bool isSquare();
    real determinant();
    IMatrix inverse();
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
    void multiply(real value)
    {
        foreach (inout row; rows_)
            foreach (inout v; row)
                v *= value;
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

    FullMatrix inverse()
    {
        if (!isSquare)
            throw new Exception("Matrix inversion is only possible for square matrices.");
        switch (nrRows)
        {
            case 1:
                {
                    real v = rows_[0][0];
                    if (v == 0)
                        throw new Exception("Could not invert matrix");
                    return new SingleValueMatrix(1.0/rows_[0][0]);
                }
                break;
            case 2:
                throw new Exception("Not implemented yet.");
            default:
                throw new Exception("We currently don't support this dimension.");
                break;
        }
        return null;
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
    this(real[] diagonal)
    {
        diagonal_ = diagonal.dup;
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
    void multiply(real value)
    {
        foreach (inout d; diagonal_)
            d *= value;
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
    DiagonalMatrix inverse()
    {
        real[] invertedDiagonal = diagonal_.dup;
        foreach (inout v; invertedDiagonal)
        {
            if (v == 0)
                throw new Exception("Could not invert diagonal matrix.");
            v = 1.0/v;
        }
        return new DiagonalMatrix(invertedDiagonal);
    }

    private:
    real[] diagonal_;
}

class SingleValueMatrix: IMatrix
{
    public:
    this()
    {
        value_ = 0.0;
    }
    this(real value)
    {
        value_ = value;
    }

    //IMatrix interface
    uint nrRows() { return 1; }
    uint nrCols() { return 1; }
    real[] opMul(real[] rhs)
    {
        return [value_*rhs[0]];
    }
    void multiply(real[] res, real[] rhs)
    {
        res[0] = value_*rhs[0];
    }
    void multiply(real value)
    {
        value_ *= value;
    }
    real leftRightProduct(real[] vector)
    {
        return vector[0]*vector[0]*value_;
    }
    bool isSquare() { return true; }
    real determinant() { return value_; }
    SingleValueMatrix inverse()
    {
        if (value_ == 0)
            throw new Exception("Could not invert single valued matrix");
        return new SingleValueMatrix(1.0/value_);
    }

    private:
    real value_;
}

IMatrix createIdentity(uint nr)
{
    if (nr == 1)
        return new SingleValueMatrix(1.0);

    DiagonalMatrix res = new DiagonalMatrix(nr);
    for (uint i = 0; i < nr; ++i)
        res.diagonal_[i] = 1.0;
    return res;
}


version (UnitTest)
{
    import gubg.Puts;

    void main()
    {
        auto m = new FullMatrix(3,5);
        auto id = createIdentity(5);
        real[] v = [1,2,3,4,5];
        puts("m.nrCols = {}", m.nrCols);
        puts("res = {}", m*v);
        puts("res = {}", id*v);
    }
}
