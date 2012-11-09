#ifndef HEADER_gubg_mask_vectorMask_hpp_ALREADY_INCLUDED
#define HEADER_gubg_mask_vectorMask_hpp_ALREADY_INCLUDED

#include <vector>
#include <iostream>

#include "random.hpp"

using namespace std;

namespace gubg
{
    class VectorMask
    {
    public:
        // start and end indices are included in the internal set (if it makes sense)
        VectorMask(int start = -1, int end = -2):
            mStart(start),
            mEnd(end){}
    
        void setStartEnd(int start, int end)
            {
                mStart = start;
                mEnd = end;
            }

        template <typename T>
        bool filter(vector<T> &internal, vector<T> &external, const vector<T> &source, bool truncateBefore = true)
            {
                if (truncateBefore)
                {
                    internal.resize(0);
                    external.resize(0);
                }
                for (int i=0; i<source.size(); ++i)
                    if (mStart <= i && i <= mEnd)
                        internal.push_back(source[i]);
                    else
                        external.push_back(source[i]);
                return true;
            }

        void perturbate(int length)
            {
                int b,t;
                b = (mStart<0      ? 1 : (mStart==0      ? 0 : -1));
                t = (mStart<length ? 1 : (mStart==length ? 0 : -1));
                mStart += Random::drawUniformDiscrete(b, t);
                b = (mEnd<mStart-1 ? 1 : (mEnd==mStart-1 ? 0 : -1));
                t = (mEnd<length-1 ? 1 : (mEnd==length-1 ? 0 : -1));
                mEnd   += Random::drawUniformDiscrete(b, t);
            }

        void print()
            {
                cout << "VM: start = " << mStart << ", end = " << mEnd << "" << endl;
            }
    private:
        int mStart;
        int mEnd;
    };
}

#endif
