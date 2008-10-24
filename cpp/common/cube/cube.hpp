#ifndef cube_h
#define cube_h

#include <cmath>
#include <vector>
#include <iostream>

#include "cubeIter.hpp"

// The Cube namespace collects functionality that has to do with multidimensional vectors
namespace Cube
{
    template <typename D, typename S>
    bool setStructure(D &dest, const S &src)
    {
        return true;
    }
    template <typename D, typename S>
    bool setStructure(vector<D> &dest, const vector<S> &src)
    {
        dest.resize(src.size());
        if (Dimension<D>::dimension > 0)
        {
            bool ok = true;
            for (long i=0; ok && i<src.size(); i++)
                ok = setStructure(dest[i], src[i]);
            return ok;
        }
        return true;
    }

    template <typename T, typename U, typename V>
    bool setStructure(T &dest, const U &src, const V &src2)
    {
        return setStructure(dest,src2);
    }
    template <typename T, typename U, typename V>
    bool setStructure(vector<T> &dest, const vector<U> &src, const V &src2)
    {
        dest.resize(src.size());
        if (Dimension<U>::dimension > 0)
        {
            bool ok = true;
            for (long i=0; ok && i<src.size(); i++)
                ok = setStructure(dest[i],src[i],src2);
            return ok;
        } else
        {
            bool ok = true;
            for (long i=0; ok && i<src.size(); i++)
                ok = setStructure(dest[i],src[i],src2);
            return ok;
        }
    }

    template <typename D, typename S>
    bool add(D &d, const S &s, typename Dimension<D>::basetype factor = 1)
    {
        Iterator<typename Dimension<D>::basetype,Dimension<D>::dimension> iterD(d);
        Iterator<typename Dimension<S>::basetype,Dimension<S>::dimension> iterS(*const_cast<S*>(&s));
        for (; iterD.valid() && iterS.valid() ; ++iterD,++iterS)
            *iterD += (*iterS)*factor;
        return true;
    }

    template <typename D, typename S>
    bool diff(D &d, const S &s)
    {
        Iterator<typename Dimension<D>::basetype,Dimension<D>::dimension> iterD(d);
        Iterator<typename Dimension<S>::basetype,Dimension<S>::dimension> iterS(*const_cast<S*>(&s));
        for (; iterD.valid() && iterS.valid() ; ++iterD,++iterS)
            *iterD -= (*iterS);
        return true;
    }

    template <typename T>
    bool computeMeanSigma(double &mean, double &sigma, const T &cube)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(*const_cast<T*>(&cube));
        long nrElements = 0;
        typename Dimension<T>::basetype sum = 0;
        for (; iter.valid(); ++iter)
        {
            ++nrElements;
            sum += (*iter);
        }
        if (nrElements<2)
        {
            cerr << "You need at least two values to compute a standard deviation" << endl;
            return false;
        }
        mean = sum/double(nrElements);
        sigma = 0;
        for (iter.set(*const_cast<T*>(&cube)); iter.valid(); ++iter)
            sigma += (*iter-mean)*(*iter-mean);
        sigma /= nrElements-1.0;
        sigma = sqrt(sigma);
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<vector<T> > &lhs, const vector<vector<T> > &rhs)
    {
        int nrR = lhs.size(), nrK = lhs[0].size(), nrC = rhs[0].size();
        if (rhs.size() != nrK)
        {
            cerr << "Number of cols of lhs should equal number of rows of rhs for multiplyRight(2,2,2): (" << rhs.size() << "," << nrK << ")" << endl;
            return false;
        }
        res.resize(nrR);
        for (int i = 0; i<nrR; ++i)
        {
            res[i].resize(nrC);
            for (int j = 0; j<nrC; ++j)
            {
                T inp = 0.0;
                for (int k = 0; k<nrK; ++k)
                    inp += lhs[i][k]*rhs[k][j];
                res[i][j] = inp;
            }
        }
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<vector<vector<T> > > &lhs, const vector<T> &rhs)
    {
        int nrR = lhs.size(), nrR2 = lhs[0].size(), nrK = lhs[0][0].size();
        if (rhs.size() != nrK)
        {
            cerr << "Number of cols of lhs should equal number of rows of rhs for multiplyRight(2,3,1): (" << rhs.size() << "," << nrK << ")" << endl;
            return false;
        }
        res.resize(nrR);
        for (int i = 0; i<nrR; ++i)
        {
            res[i].resize(nrR2);
            for (int j = 0; j<nrR2; ++j)
            {
                T inp = 0.0;
                for (int k = 0; k<nrK; ++k)
                    inp += lhs[i][j][k]*rhs[k];
                res[i][j] = inp;
            }
        }
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<T> &lhs, const vector<vector<vector<T> > > &rhs)
    {
        int nrK = lhs.size(), nrC = rhs[0].size(), nrC2 = rhs[0][0].size();
        if (rhs.size() != nrK)
        {
            cerr << "Number of cols of lhs should equal number of rows of rhs for multiplyRight(2,1,3): (" << rhs.size() << "," << nrK << ")" << endl;
            return false;
        }
        res.resize(nrC);
        for (int i = 0; i<nrC; ++i)
        {
            res[i].resize(nrC2);
            for (int j = 0; j<nrC2; ++j)
            {
                T inp = 0.0;
                for (int k = 0; k<nrK; ++k)
                    inp += lhs[k]*rhs[k][i][j];
                res[i][j] = inp;
            }
        }
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<T> &res, const vector<vector<T> > &lhs, const vector<T> &rhs)
    {
        int nrR = lhs.size(), nrK = lhs[0].size();
        if (rhs.size() != nrK)
        {
            cerr << "Number of cols of lhs should equal number of rows of rhs for multiplyRight(1,2,1): (" << rhs.size() << "," << nrK << ")" << endl;
            return false;
        }
        res.resize(nrR);
        for (int i = 0; i<nrR; ++i)
        {
            T inp = 0.0;
            for (int k = 0; k<nrK; ++k)
                inp += lhs[i][k]*rhs[k];
            res[i] = inp;
        }
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<T> &res, const vector<T> &lhs, const vector<vector<T> > &rhs)
    {
        int nrC = rhs[0].size(), nrK = lhs.size();
        if (rhs.size() != nrK)
        {
            cerr << "Number of cols of lhs should equal number of rows of rhs for multiplyRight(1,1,2): (" << rhs.size() << "," << nrK << ")" << endl;
            return false;
        }
        res.resize(nrC);
        for (int i = 0; i<nrC; ++i)
        {
            T inp = 0.0;
            for (int k = 0; k<nrK; ++k)
                inp += lhs[k]*rhs[k][i];
            res[i] = inp;
        }
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<T> > > &res, const vector<vector<vector<T> > > &lhs, const vector<vector<T> > &rhs)
    {
        int nrR = lhs.size(), nrR2 = lhs[0].size(), nrK = lhs[0][0].size(), nrC = rhs[0].size();
        if (rhs.size() != nrK)
        {
            cerr << "Number of cols of lhs should equal number of rows of rhs for multiplyRight(3,3,2): (" << rhs.size() << "," << nrK << ")" << endl;
            return false;
        }
        res.resize(nrR);
        for (int i = 0; i<nrR; ++i)
        {
            res[i].resize(nrR2);
            for (int j = 0; j<nrR2; ++j)
            {
                res[i][j].resize(nrC);
                for (int l = 0; l<nrC; ++l)
                {
                    T inp = 0.0;
                    for (int k = 0; k<nrK; ++k)
                        inp += lhs[i][j][k]*rhs[k][l];
                    res[i][j][l] = inp;
                }
            }
        }
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<T> > > &res, const vector<vector<T> > &lhs, const vector<vector<vector<T> > > &rhs)
    {
        int nrR = lhs.size(), nrK = lhs[0].size(), nrC = lhs[0].size(), nrC2 = rhs[0][0].size();
        if (rhs.size() != nrK)
        {
            cerr << "Number of cols of lhs should equal number of rows of rhs for multiplyRight(3,2,3): (" << rhs.size() << "," << nrK << ")" << endl;
            return false;
        }
        res.resize(nrR);
        for (int i = 0; i<nrR; ++i)
        {
            res[i].resize(nrC);
            for (int j = 0; j<nrC; ++j)
            {
                res[i][j].resize(nrC2);
                for (int l = 0; l<nrC2; ++l)
                {
                    T inp = 0.0;
                    for (int k = 0; k<nrK; ++k)
                        inp += lhs[i][k]*rhs[k][j][l];
                    res[i][j][l] = inp;
                }
            }
        }
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<T> &res, const T &lhs, const vector<T> &rhs)
    {
        res.resize(rhs.size());
        for (int i=0; i<rhs.size(); ++i)
            res[i] = lhs*rhs[i];
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<T> &res, const vector<T> &lhs, const T &rhs)
    {
        res.resize(lhs.size());
        for (int i=0; i<lhs.size(); ++i)
            res[i] = lhs[i]*rhs;
        return true;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<T> &lhs, const vector<vector<T> > &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<vector<T> > > > &res, const vector<vector<vector<vector<T> > > > &lhs, const vector<vector<T> > &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<T> > > &res, const vector<vector<T> > &lhs, const vector<T> &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<T> > > &res, const vector<vector<vector<vector<T> > > > &lhs, const vector<vector<T> > &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<T> > > &res, const vector<T> &lhs, const vector<vector<T> > &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<vector<vector<vector<T> > > > &lhs, const vector<T> &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<T> &lhs, const vector<vector<vector<vector<T> > > > &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<T> > > &res, const vector<vector<vector<T> > > &lhs, const vector<T> &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<vector<T> > &lhs, const vector<T> &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<T> > &res, const vector<T> &lhs, const vector<T> &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<T> &res, const vector<T> &lhs, const vector<T> &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiplyRight(vector<vector<vector<T> > > &res, const vector<vector<T> > &lhs, const vector<vector<vector<vector<T> > > > &rhs)
    {
        cerr << "This multiplyRight is not implemented and might not make any sense" << endl;
        return false;
    }

    template <typename T>
    bool multiply2All(T &cube, typename Dimension<T>::basetype factor)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(cube);
        for (; iter.valid() ; ++iter)
            *iter *= factor;
        return true;
    }

    template <typename T>
    bool add2All(T &cube, typename Dimension<T>::basetype addition)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(cube);
        for (; iter.valid() ; ++iter)
            *iter += addition;
        return true;
    }

    template <typename T>
    bool setSame(T &cube, typename Dimension<T>::basetype value)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(cube);
        for (; iter.valid() ; ++iter)
            *iter = value;
        return true;
    }

    template <typename T>
    long nrElements(const T &cube)
    {
        long nr=0;
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(*const_cast<T*>(&cube));
        for (; iter.valid() ; ++iter)
            ++nr;
        return nr;
    }

    template <typename T>
    bool flatten(vector<typename Dimension<T>::basetype> &v, const T &cube)
    {
        long nrE = nrElements(cube);
        v.resize(nrE);
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(*const_cast<T*>(&cube));
        for (int i=0; iter.valid() ; ++iter,++i)
            v[i] = *iter;
        return true;
    }

    template <typename T, typename B>
    bool update(T &cubeDest, const T &cubeSrc, B factor = 1)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iterD(cubeDest);
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iterS(*const_cast<T*>(&cubeSrc));
        for (; iterD.valid() && iterS.valid() ; ++iterD,++iterS)
            *iterD += factor*(*iterS);
        if (iterD.valid() || iterS.valid())
        {
            cerr << "Number of elements of both cubes is not the same in Cube::update" << endl;
            return false;
        }
        return true;
    }

    template <typename T, typename R>
    bool inproduct(R &res, const T &cubeLhs, const T &cubeRhs)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iterL(*const_cast<T*>(&cubeLhs));
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iterR(*const_cast<T*>(&cubeRhs));
        res = 0;
        for (; iterL.valid() && iterR.valid() ; ++iterL,++iterR)
            res += (*iterL)*(*iterR);
        if (iterR.valid() || iterL.valid())
        {
            cerr << "Number of elements of lhs and rhs is not the same in Cube::inproduct" << endl;
            return false;
        }
        return true;
    }

    template <typename T>
    double l2Norm(const T &cube)
    {
        double res = 0.0;
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(*const_cast<T*>(&cube));
        for (; iter.valid() ; ++iter)
            res += (*iter)*(*iter);
        res = sqrt(res);
        return res;
    }

    template <typename T>
    double l2Norm2(const T &cube)
    {
        double res = 0.0;
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(*const_cast<T*>(&cube));
        for (; iter.valid() ; ++iter)
            res += (*iter)*(*iter);
        return res;
    }

    template<typename T>
    bool findMinMax(typename Dimension<T>::basetype &mi, typename Dimension<T>::basetype &ma, const T &cube)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(*const_cast<T*>(&cube));
        if (!iter.valid())
            return false;
        mi = ma = *iter;
        for (; iter.valid() ; ++iter)
            if (*iter < mi)
                mi = *iter;
            else if (*iter > ma)
                ma = *iter;
        return true;
    }
    
    template <typename D, typename S>
    bool convert(D &d, const S &s)
    {
        setStructure(d, s);
        Iterator<typename Dimension<D>::basetype,Dimension<D>::dimension> iterD(d);
        Iterator<typename Dimension<S>::basetype,Dimension<S>::dimension> iterS(*const_cast<S*>(&s));
        for ( ; iterD.valid() && iterS.valid() ; ++iterD,++iterS)
            *iterD = *iterS;
        if (iterD.valid() || iterS.valid())
        {
            cerr << "Both cubes do not contain the same number of elements in convert()" << endl;
            return false;
        }
        return true;
    }
    
    template <typename T>
    bool transformLinear(T &cube, double a, double b)
    {
        Iterator<typename Dimension<T>::basetype,Dimension<T>::dimension> iter(cube);
        for ( ; iter.valid() ; ++iter)
            *iter = a*(*iter)+b;
        return true;
    }

    template <typename T>
    bool transpose(vector<vector<T> > &res, vector<vector<T> > &orig)
    {
        int nrR = orig.size(), nrC = orig[0].size();
        res.resize(nrC);
        for (int i=0; i<nrR; ++i)
        {
            for (int j=0; j<nrC; ++j)
            {
                if (i==0)
                    res[j].resize(nrR);
                res[j][i] = orig[i][j];
            }
        }
    }
};

#endif
