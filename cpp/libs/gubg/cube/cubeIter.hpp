#ifndef cubeIter_h
#define cubeIter_h

// Iteration over cubes

#include <vector>
#include <iostream>

#include "cubeDimension.hpp"

namespace gubg
{
// The Cube namespace collects functionality that has to do with multidimensional vectors
// The top Iterator (least nested) contains the most nested vector
    namespace Cube
    {
        template <typename BaseType, int dim>
        class Iterator
        {
        public:
            Iterator(){}
            template <typename U>
            Iterator(vector<U> &u)
                {
                    set(u);
                }
            template <typename U>
            BaseType *set(vector<U> &u)
                {
                    mIX = 0;
                    vector<BaseType> *ary = mNextDimIter.set(u);
                    if (ary)
                    {
                        mPtr = &(*ary)[0];
                        mLength = ary->size();
                    } else
                    {
                        mPtr = NULL;
                        mLength = 0;
                    }
                    return mPtr;
                }
            template <typename U>
            BaseType *change(vector<U> &u)
                {
                    vector<BaseType> *ary = mNextDimIter.change(u);
                    if (!ary)
                    {
                        cerr << "You are changing a cube but I received a NULL pointer (dim = " << dim << ")" << endl;
                        return NULL;
                    }
                    long lSize = ary->size();
                    if (mLength != lSize || !valid())
                    {
                        cerr << "You are changing a cube but the structure is not the same (dim = " << dim << ") (" << mLength << " != " << u.size() << ") or it is not valid (" << valid() << ")" << endl;
                        return NULL;
                    }
                    mPtr = &(*ary)[0];
                    return &mPtr[mIX];
                }
            BaseType &operator*()
                {
                    return mPtr[mIX];
                }
            Iterator &operator++()
                {
                    ++mIX;
                    while (mIX >= mLength)
                    {
                        ++mNextDimIter;
                        if (mNextDimIter.valid())
                        {
                            mIX = 0;
                            mLength = (*mNextDimIter).size();
                            if (mLength > 0)
                                mPtr = &(*mNextDimIter)[0];
                            else
                                mPtr = NULL;
                        } else break;
                    }
                    return *this;
                }
            bool valid()
                {
                    return mIX<mLength;
                }
        private:
            BaseType *mPtr;
            long mIX;
            long mLength;
            Iterator<vector<BaseType>,dim-1> mNextDimIter;
        };

        template <typename BaseType>
        class Iterator<BaseType, 1>
        {
        public:
            Iterator(){}
            Iterator(vector<BaseType> &u)
                {
                    set(u);
                }
            BaseType *set(vector<BaseType> &u)
                {
                    mIX = 0;
                    if (u.size() > 0)
                    {
                        mPtr = &u[0];
                        mLength = u.size();
                    } else
                    {
                        mPtr = NULL;
                        mLength = 0;
                    }
                    return mPtr;
                }
            BaseType *change(vector<BaseType> &u)
                {
                    long lSize = u.size();
                    if (mLength != lSize || !valid())
                    {
                        cerr << "You are changing a cube but the structure is not the same (dim = 1) (" << mLength << " != " << u.size() << ") or it is not valid (" << valid() << ")" << endl;
                        return NULL;
                    }
                    mPtr = &u[0];
                    return &mPtr[mIX];
                }
            BaseType &operator*()
                {
                    return mPtr[mIX];
                }
            Iterator &operator++()
                {
                    ++mIX;
                    return *this;
                }
            bool valid()
                {
                    return mIX<mLength;
                }
        private:
            BaseType *mPtr;
            long mIX;
            long mLength;
        };

        template <typename BaseType>
        class Iterator<BaseType,0>
        {
        public:
            Iterator():
                mPtr(NULL),
                mValid(false){}
            Iterator(BaseType &t)
                {
                    set(t);
                }
            BaseType *set(BaseType &t)
                {
                    mValid = true;
                    mPtr = &t;
                    return NULL;
                }
            BaseType *change(BaseType &t)
                {
                    if (!valid())
                    {
                        cerr << "You are changing a cube but the structure is not the same (dimension 0 which was previously not valid)" << endl;
                        return NULL;
                    }
                    mPtr = &t;
                    return NULL;
                }
            BaseType &operator*()
                {
                    if (mValid)
                        return *mPtr;
                    else
                        return *reinterpret_cast<BaseType*>(NULL);
                }
            Iterator &operator++()
                {
                    mValid = false;
                    return *this;
                }
            bool valid()
                {
                    return mValid;
                }
        private:
            BaseType *mPtr;
            bool mValid;
        };
    }
}

#endif
