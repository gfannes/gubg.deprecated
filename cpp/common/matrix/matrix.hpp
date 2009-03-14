#ifndef matrix_h
#define matrix_h

#include <vector>
#include <iostream>

#include "serialize.hpp"

using namespace std;

namespace gubg
{
    template <typename T>
    class Matrix: public vector< vector<T> >
    {
    public:
	Matrix(long nrR = 0, long nrC = 0)
	    {
		setSize(nrR,nrC);
	    }
	Matrix(long nrR, long nrC, const vector<T> &values)
	    {
		setSize(nrR,nrC);
		setElements(values);
	    }
	bool setSize(long nrR, long nrC)
	    {
		this->resize(nrR);
		for (int i = 0; i<nrR; i++)
		    (*this)[i].resize(nrC);
		mNrRows = nrR;
		mNrCols = nrC;
		return true;
	    };

	vector<vector<T> > &data(){return *this;}

	long nrRows() const{return mNrRows;}
	long nrCols() const{return mNrCols;}
	bool setElement(long rix, long cix, T el)
	    {
		if (rix>=0 && rix<mNrRows && cix>=0 && cix<mNrCols)
		{
		    (*this)[rix][cix] = el;
		    return true;
		}
		cerr << "Index out of range when setting element (" << rix << "," << cix << ") to " << el << "" << endl;
		return false;
	    }
	bool setElements(const vector<T> &values)
	    {
		if (values.size() != mNrRows*mNrCols)
		{
		    cerr << "Number of elements does not correspond with matrix dimensions" << endl;
		    return false;
		}
		for (long i = 0, k = 0; i<mNrRows; i++)
		{
		    for (long j = 0; j<mNrCols; j++,k++)
			(*this)[i][j] = values[k];
		}
		return true;
	    }
	bool setElements(long rix, long cix, const Matrix<T> & elements)
	    {
		long nrR = elements.nrRows(), nrC = elements.nrCols();
		long ii = 0;
		for (long i = rix; (i < mNrRows) && (ii < nrR); ++i,++ii)
		{
		    long jj = 0;
		    for (long j = cix; (i >= 0) && (j < mNrCols) && (jj < nrC); ++j,++jj)
		    {
			if (j >= 0)
			    (*this)[i][j] = elements[ii][jj];
		    }
		}
		return true;
	    }
	template <typename U>
	Matrix &operator=(const vector<vector<U> > &rhs)
	    {
		long nrR = rhs.size(), nrC = (nrR==0 ? 0 : rhs[0].size());
		setSize(nrR,nrC);
		// We should check if rhs is a real matrix with the same elements per row ...
		for (long i = 0; i<nrR; i++)
		    for (long j = 0; j<nrC; j++)
			(*this)[i][j] = rhs[i][j];
		return (*this);
	    }
	bool writeTo(basic_ostream<char> &os) const
	    {
		for (long i = 0, k = 0; i<mNrRows; i++)
		{
		    if (i>0)
			os << endl;
		    for (long j = 0; j<mNrCols; j++,k++)
		    {
			if (j>0)
			    os << "\t";
			os << (*this)[i][j];
		    }
		}
		return true;
	    }
	// Serialization
	static Matrix* allocate4Serialize(){return new Matrix<T>(0,0);}
	bool serialize4Serialize(ostream &output)
	    {
                Serialize::serialize(mNrRows,output);
                Serialize::serialize(mNrCols,output);
                for (long i=0;i<mNrRows;i++)
                    for (long j=0;j<mNrCols;j++)
                        if (!Serialize::serialize((*this)[i][j],output))
                        {
                            cerr << "serialize4Serialize failed." << endl;
                            return false;
                        }
                return true;
	    }
	bool unserialize4Serialize(istream &input)
	    {
		long nrR,nrC;
		if (!Serialize::unserialize(nrR,input))
		{
		    cerr << "Could not read the number of rows." << endl;
		    return false;
		}
		if (!Serialize::unserialize(nrC,input))
		{
		    cerr << "Could not read the number of cols." << endl;
		    return false;
		}
		setSize(nrR,nrC);
		for (long i=0;i<mNrRows;i++)
		    for (long j=0;j<mNrCols;j++)
			if (!Serialize::unserialize((*this)[i][j],input))
			{
			    cerr << "unserialize4Serialize failed." << endl;
			    return false;
			}
		return true;
	    }

	bool match(vector<T> &res, const Matrix<bool> &mask, long rix, long cix)
	    {
		if (rix<0 || cix<0 || rix+mask.nrRows()>=mNrRows || cix+mask.nrCols()>=mNrCols)
		{
		    cerr << "Index is out of range when matching a mask on a matrix" << endl;
		    return false;
		}
		res.resize(0);
		for (long i = 0; i<mask.nrRows(); i++)
		    for (long j = 0; j<mask.nrCols(); j++)
			if (mask[i][j])
			    res.push_back((*this)[i+rix][j+cix]);
		return true;
	    }
    private:
	long mNrRows;
	long mNrCols;
    };

    template <typename T>
    basic_ostream<char> &operator<<(basic_ostream<char> &os, const Matrix<T> &matrix)
    {
	matrix.writeTo(os);
	return os;
    }
}

#include "typeIDs.hpp"

namespace gubg
{
    namespace Serialize
    {
	SetType(Matrix<int>,eTypeIDMatrixInt);
	SetType(Matrix<long>,eTypeIDMatrixLong);
	SetType(Matrix<double>,eTypeIDMatrixDouble);
	SetTypeAndFunctions(vector<Matrix<int> >,eTypeIDVectorMatrixInt,BasicMethodsVector<Matrix<int> >::allocate,BasicMethodsVector<Matrix<int> >::serialize,BasicMethodsVector<Matrix<int> >::unserialize);
	SetTypeAndFunctions(vector<Matrix<long> >,eTypeIDVectorMatrixLong,BasicMethodsVector<Matrix<long> >::allocate,BasicMethodsVector<Matrix<long> >::serialize,BasicMethodsVector<Matrix<long> >::unserialize);
	SetTypeAndFunctions(vector<Matrix<double> >,eTypeIDVectorMatrixDouble,BasicMethodsVector<Matrix<double> >::allocate,BasicMethodsVector<Matrix<double> >::serialize,BasicMethodsVector<Matrix<double> >::unserialize);
    }
}

#endif
