#ifndef HEADER_gubg_network_edge_hpp_ALREADY_INCLUDED
#define HEADER_gubg_network_edge_hpp_ALREADY_INCLUDED

#include <iostream>
#include <sstream>

using namespace std;

namespace gubg
{
    class Vertex;

    class Edge
    {
    public:
        Edge(Vertex *start, Vertex *end):
            mStart(start),
            mEnd(end){};
        Vertex *start(){return mStart;};
        Vertex *end(){return mEnd;};
        Vertex *otherSide(Vertex *thisSide){return ((thisSide==mStart) ? mEnd : mStart);};
        bool connects2(Vertex *someSide){return (someSide==mStart)||(someSide==mEnd);};
        string toString()
            {
                ostringstream ostr;
                ostr << "(Edge) this = " << this << " mStart = " << mStart << " mEnd = " << mEnd;
                return ostr.str();
            }
    private:
        Vertex *mStart;
        Vertex *mEnd;
    };
}

#endif
