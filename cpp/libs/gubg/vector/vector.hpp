#ifndef HEADER_gubg_vector_vector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_vector_vector_hpp_ALREADY_INCLUDED

#include "random.hpp"
#include "math.hpp"
#include "cube.hpp"
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstdarg>
#include <cmath>
#include <iostream>
#include <utility>

namespace gubg
{
    using namespace Cube;

    namespace Vector
    {
        template< typename T >
        bool inprod(const vector<T> &lhs,const vector<T> &rhs,T &t)
        {
            int nr=lhs.size();
            if (nr!=rhs.size())
            {
                cerr << "The size of both vectors is not the same when computing an inproduct " << nr << " != " << rhs.size() << endl;
                return false;
            }
            t=0;
            for (nr--;nr>=0;nr--)
                t += lhs[nr]*rhs[nr];
            return true;
        }

        template<typename T>
        bool setUniform(vector<T> &vec,long nr,double min,double max, bool initRandom=true)
        {
            vec.resize(nr);
            long i;
            if (initRandom)
                Random::initialize();
            for (i=0;i<nr;i++)
                vec[i] = Random::drawUniform(min,max);
            return true;
        }

        template<typename T>
        bool setIncremental(vector<T> &vec,long nr,T start,T delta)
        {
            vec.resize(nr);
            for (long i = 0 ; i < nr ; i++, start+=delta)
                vec[i] = start;
            return true;
        }

        template<typename T>
        bool setEqual(vector<T> &vec,long nr,double min,double max)
        {
            vec.resize(nr);
            long i;
            if (1 == nr)
                vec[0] = 0.5*(min+max);
            else if (nr > 1)
            {
                double delta=(max-min)/(nr-1);
                for (i=0;i<nr;i++)
                    vec[i] = min+i*delta;
            } else
                return false;
            return true;
        }

        template<typename T>
        bool setSame(vector<T> &vec,long nr,T value)
        {
            vec.resize(nr);
            long i;
            for (i=0;i<nr;i++)
                vec[i] = value;
            return true;
        }
   
//begin template
//R (1..10).each do |i|
//R   args=(1..i).collect{|j|"T v#{j}"}.join(',')
//R   pbs=(1..i).collect{|j|"vec.push_back(v#{j});"}.join(" ")
//C    template<typename T>
//C    bool set(vector<T> &vec,#{args})
//C    {
//C      vec.resize(0);
//C      #{pbs}
//C      return true;
//C    }
//R end

        template<typename T>
        bool set(vector<T> &vec,T v1)
        {
            vec.resize(0);
            vec.push_back(v1);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3,T v4)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3); vec.push_back(v4);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3,T v4,T v5)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3); vec.push_back(v4); vec.push_back(v5);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3,T v4,T v5,T v6)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3); vec.push_back(v4); vec.push_back(v5); vec.push_back(v6);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3,T v4,T v5,T v6,T v7)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3); vec.push_back(v4); vec.push_back(v5); vec.push_back(v6); vec.push_back(v7);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3); vec.push_back(v4); vec.push_back(v5); vec.push_back(v6); vec.push_back(v7); vec.push_back(v8);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8,T v9)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3); vec.push_back(v4); vec.push_back(v5); vec.push_back(v6); vec.push_back(v7); vec.push_back(v8); vec.push_back(v9);
            return true;
        }

        template<typename T>
        bool set(vector<T> &vec,T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8,T v9,T v10)
        {
            vec.resize(0);
            vec.push_back(v1); vec.push_back(v2); vec.push_back(v3); vec.push_back(v4); vec.push_back(v5); vec.push_back(v6); vec.push_back(v7); vec.push_back(v8); vec.push_back(v9); vec.push_back(v10);
            return true;
        }

//end template

        // vector * any
        template< typename T, typename T2 >
        bool multiplyOuter(vector<T> &lhs,T2 &rhs,vector<T2> &res)
        {
            cout << "multiplyOuter vector * any" << endl;
            int i,len=lhs.size();
            res.resize(len);
            for (i=0;i<len;i++)
            {
                cout << i << endl;
                if (!multiplyOuter(lhs[i],rhs,res[i]))
                {
                    cerr << "Could not multiplyOuter row " << i << "with rhs" << endl;
                    return false;
                }
            }
            return true;
        }
        // scalar * vector
        template< typename T, typename T2 >
        bool multiplyOuter(T &lhs, vector<T2> &rhs, vector<T2> &res)
        {
            cout << "multiplyOuter scalar * any" << endl;
            int i,len=rhs.size();
            res.resize(len);
            for (i=0;i<len;i++)
            {
                cout << "\t" << i << endl;
                if (!multiplyOuter(lhs,rhs[i],res[i]))
                {
                    cerr << "Could not multiplyOuter lhs with col " << i << endl;
                    return false;
                }
            }
            return true;
        }
        template< typename T >
        bool multiplyOuter(T &lhs, T &rhs, T &res)
        {
            cout << "multiplyOuter scalar * scalar" << endl;
            res=lhs*rhs;
            return true;
        }

        template<typename T, int dimension>
        struct OutputMethods
        {
            template<typename T2>
            static string toString(const vector<T2> &v,int indent = 0)
                {
                    ostringstream res;
                    int i,j;
                    for (i=0 ; i<v.size() ; i++)
                    {
                        for (j=0;j<indent;j++)
                            res << "  ";
                        res << "index<" << indent << "> == " << i << endl;
                        res << OutputMethods<T2,dimension-1>::toString(v[i],indent+1);
                    }
                    return res.str();
                }
        };
        template<typename T>
        struct OutputMethods<T,0>
        {
            static string toString(const T &v,int indent = 0)
                {
                    ostringstream res;
                    for (;indent>0;indent--)
                        res << "  ";
                    res << v;
                    return res.str();
                }
        };
        template<typename T>
        struct OutputMethods<T,1>
        {
            static string toString(const T &v,int indent = 0)
                {
                    typename T::const_iterator it;
                    ostringstream res;
                    for (;indent>0;indent--)
                        res << "  ";
                    res << "[";
                    for (it=v.begin() ; it!=v.end() ; it++)
                    {
                        if (it==v.begin())
                        {
                            res << *it;
                        } else
                        {
                            res << ", " << *it;
                        }
                    }
                    res << "]";
                    return res.str();
                }
        };
        template<typename T>
        struct OutputMethods<vector<T>,2>
        {
            static string toString(const vector<T> &v,int indent = 0)
                {
                    typename vector<T>::const_iterator it;
                    ostringstream res;
                    for (int i=0 ; i<v.size() ; ++i)
                    {
                        for (int j=0; j<indent; ++j)
                            res << "  ";
                        if (i==0)
                            res << "[";
                        else
                            res << " ";
                        res << OutputMethods<T,1>::toString(v[i],0);
                        if (i==v.size()-1)
                            res << "]";
                        res << endl;
                    }
                    return res.str();
                }
        };

        template< typename T >
        string toString(const T &v,int indent = 0)
        {
            return OutputMethods<T,Dimension<T>::dimension>::toString(v,indent);
        }

        // methods to iterate through nested vectors
        template <typename T>
        bool initIter(vector<int> &path, const T &v)
        {
            if (Dimension<T>::dimension == 0)
            {
                path.resize(1);
                path[0] = -1;
            } else
            {
                path.resize(Dimension<T>::dimension);
                int i;
                for (i=0;i<Dimension<T>::dimension-1;i++)
                    path[i]=0;
                path[Dimension<T>::dimension-1] = -1;
            }
            return true;
        }

        template <typename B, int proceed, int diff>
        struct ProceedIter
        {
            template <typename V>
            static bool lProceedIter(vector<int> &path, vector<V> &v, B *&p, int pathIX)
                {
                    // cout << "lProceed (general) with &v = " << &v << endl; // do(1)
                    int &ix = path[pathIX];
                    if (!proceed && ix>=v.size())
                    {
                        v.resize(ix+1);
                        if (ix>=v.size())
                        {
                            cerr << "Could not expand the v to size " << ix+1 << endl;
                            return false;
                        }
                    }
                    if ( ProceedIter<B, proceed, diff-1 >::lProceedIter(path,v[ix],p,pathIX+1) )
                    {
                        return true;
                    } else
                    {
                        if (proceed)
                        {
                            ix++;
                            if (ix>=v.size())
                            {
                                ix=0;
                                return false;
                            }
                            return ProceedIter<B,0,diff-1>::lProceedIter(path,v[ix],p,pathIX+1);
                        } else
                            return false;
                    }
                }
        };
    
        template <typename B, int proceed>
        struct ProceedIter<B, proceed, 1>
        {
            static bool lProceedIter(vector<int> &path, vector<B> &v, B *&p, int pathIX)
                {
                    // cout << "lProceed (dim == 1) with &v = " << &v << " and size == " << v.size() << endl; // do(1)
                    int &ix = path[pathIX];
                    if (proceed)
                    {
                        ix++;
                        if (ix>=v.size())
                        {
                            ix=0;
                            return false;
                        }
                        p = &v[ix];
                        return true;
                    } else
                    {
                        if (ix>=v.size())
                            v.resize(ix+1);
                        if (ix<v.size())
                        {
                            p = &v[ix];
                            return true;
                        }
                        cerr << "Could not expand the v to size " << ix+1 << " (" << v.size() << ")" << endl;
                        return false;
                    }
                }
        };
    
        template <typename B, int proceed>
        struct ProceedIter<B, proceed, 0>
        {
            static bool lProceedIter(vector<int> &path, B &v, B *&p, int pathIX)
                {
                    // cout << "lProceed (dim == 0) with &v = " << &v << endl; // do(1)
                    int &ix = path[pathIX];
                    if (ix > 0)
                        return false;
                    if (proceed)
                        ix++;
                    if (ix == 0)
                    {
                        p = &v;
                        return true;
                    }
                    return false;
                }
        };
        template <typename V, typename B>
        bool proceedIter(vector<int> &path, V &v, B *&p)
        {
            bool ok = ProceedIter<B,1,Dimension<V>::dimension-Dimension<B>::dimension>::lProceedIter(path,v,p,0);
            return ok;
        }
        template <typename V, typename B>
        bool getAtIter(vector<int> &path, V &v, B *&p)
        {
            bool ok = ProceedIter<B,0,Dimension<V>::dimension-Dimension<B>::dimension>::lProceedIter(path,v,p,0);
            return ok;
        }

        template <typename T>
        bool multiply2All(vector<T> &vec, T factor)
        {
            for (long i = 0 ; i<vec.size(); ++i)
                vec[i] *= factor;
            return true;
        }

        template <typename T>
        bool add2All(vector<T> &vec, T addition)
        {
            for (long i = 0 ; i<vec.size(); ++i)
                vec[i] += addition;
            return true;
        }

        template <typename T>
        bool add(vector<T> &d,const vector<T> &s, T factor = 1)
        {
            for (long i = 0; i<d.size(); ++i)
                d[i] += s[i]*factor;
            return true;
        }

        template <typename T>
        bool diff(vector<T> &d,const vector<T> &s)
        {
            for (long i = 0; i<d.size(); ++i)
                d[i] -= s[i];
            return true;
        }

        template<typename T>
        bool findMinMax(typename Dimension<T>::basetype &mi, typename Dimension<T>::basetype &ma, vector<T> &vec)
        {
            if (vec.size() == 0)
                return false;
            mi = ma = vec[0];
            for (int i=0; i<vec.size(); ++i)
                if (vec[i] < mi)
                    mi = vec[i];
                else if (vec[i] > ma)
                    ma = vec[i];
            return true;
        }

        template <typename T>
        bool computeSum(T &sum, vector<T> &values)
        {
            sum = 0;
            for (long i=0;i<values.size();i++)
                sum += values[i];
            return true;
        }

        template <typename T>
        bool standardizeUsing(vector<double> &res, vector<T> &values, double mean, double sigma)
        {
            res.resize(values.size());
            for (long i=0;i<values.size();i++)
                res[i] = (values[i]-mean)/sigma;
            return true;
        }

        template <typename T>
        bool direction(vector<T> &res, const vector<T> &start, const vector<T> &end)
        {
            res = end;
            return diff(res,start);
        }

        template <typename T>
        bool tangent(vector<T> &res, const vector<T> &start, const vector<T> &end)
        {
            if (start.size() != 2)
            {
                cerr << "Can only compute this for vectors of length 2." << endl;
                return false;
            }
            direction(res,start,end);
            T tmp = res[0];
            res[0] = res[1];
            res[1] = -tmp;
            tmp = 1.0/l2Norm(res);
            multiply2All(res,tmp);
            return true;
        }

        template <typename T>
        T angle(const vector<T> &v1, const vector<T> &v2)
        {
            T angle = atan2(v1[1],v1[0]) - atan2(v2[1],v2[0]);
            while (angle<0)
                angle += 2*Math::PI;
            return angle;
        }

        template <typename T>
        bool sort(vector<T> &vec)
        {
            sort(vec.begin(),vec.end());
            return true;
        }

        // Sort vec2Sort and perform the same shuffling operations on attachedVec
        template <typename PairT>
        static bool smallerThan(PairT lhs, PairT rhs){return lhs.first < rhs.first;}
        template <typename PairT>
        static bool greaterThan(PairT lhs, PairT rhs){return lhs.first > rhs.first;}
        template <typename T, typename TT>
        bool sort(vector<T> &vec2Sort, vector<TT> &attachedVec, bool ascending = true)
        {
            if (vec2Sort.size() != attachedVec.size())
            {
                cout << "I expect the sizes of both arrays to be the same when sorting." << endl;
                return false;
            }

            typedef pair<T, TT> PairT;
            vector<PairT> vec;
            vec.resize(vec2Sort.size());
            for (int i=0; i<vec.size(); ++i)
            {
                vec[i].first = vec2Sort[i];
                vec[i].second = attachedVec[i];
            }

            if (ascending)
                sort(vec.begin(), vec.end(), smallerThan<PairT>);
            else
                sort(vec.begin(), vec.end(), greaterThan<PairT>);

            for (int i=0; i<vec.size(); ++i)
            {
                vec2Sort[i] = vec[i].first;
                attachedVec[i] = vec[i].second;
            }
        
            return true;
        }
    
        template <typename T>
        bool computeTrimmedMean(double &mean, const vector<T> &vec, double frac)
        {
            if (frac == 0)
                return computeMean(mean,vec);
            if (frac>0 && frac<=1)
            {
                vector<T> h = vec;
                sort(h);
                long nr = (int)(frac*(h.size()/2));
                if (frac == 1)
                    nr = (h.size()-1)/2;
                long start = nr, end = h.size()-nr;
                T sum = 0;
                for (long i=start; i<end; ++i)
                    sum += h[i];
                mean = double(sum)/double(end-start);
            } else
            {
                cerr << "The frac (" << frac << ") is out of range for computeTrimmedMean" << endl;
                return false;
            }
            return true;
        }

        template <typename T>
        bool computeMedian(double &mean, const vector<T> &vec)
        {
            return computeTrimmedMean(mean,vec,1);
        }
    }

    template<typename T>
    basic_ostream<char> &operator<<(basic_ostream<char> &os, const vector<T> &t)
    {
        os << Vector::toString(t);
        return os;
    }
}
#endif
