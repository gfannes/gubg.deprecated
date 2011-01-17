#ifndef vertex_h
#define vertex_h

#include <vector>
#include <sstream>

#include "edge.hpp"
#include "vector.hpp"

using namespace std;

namespace gubg
{
    class Vertex
    {
    public:
        Vertex():mIndex(-1){}
        bool addEdge(Edge *edge){mEdges.push_back(edge);};
        bool removeEdge(Edge *edge)
            {
                int i;
                for (i=0;i<mEdges.size();i++)
                    if (mEdges[i]==edge)
                    {
                        mEdges.erase(mEdges.begin()+i);
                        return true;
                    }
                return false;
            }

        void setIndex(long index){mIndex = index;};
        long index(){return mIndex;};

        long nrNeighbours(){return mEdges.size();};
        Vertex *getNeighbour(long i){return mEdges[i]->otherSide(this);};

        void setCo(vector<double> coodinates){mCoordinates = coodinates;};
        void setRandomCo(){Vector::setUniform(mCoordinates,2,-1,1);};
        void getCo(vector<double> &coodinates){coodinates = mCoordinates;};

        void clearForce(){Vector::set(mForce,0.0,0.0);};
        void addForce(vector<double> &force, bool reverse){Vector::add(mForce,force,(reverse ? -1.0 : 1.0));};
        void getForce(vector<double> &force){force = mForce;};

        string toString()
            {
                ostringstream ostr;
                ostr << "Vertex " << mIndex << ": ";
                int i;
                for (i=0;i<mEdges.size();i++)
                    ostr << mEdges[i]->otherSide(this)->index() << ", ";
                return ostr.str();
            }
    private:
        vector<Edge*> mEdges;
        long mIndex;
        vector<double> mCoordinates;
        vector<double> mForce;
    };
}

#endif
