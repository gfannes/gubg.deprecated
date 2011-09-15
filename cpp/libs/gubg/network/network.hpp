#ifndef network_h
#define network_h

#include <vector>
#include <string>
#include <sstream>

#include "vertex.hpp"
#include "edge.hpp"
#include "bernoulli.hpp"

using namespace std;

namespace gubg
{
    class MutationCfg
    {
    public:
        MutationCfg():
            nodeAddProb(0.5),
            nodeDeleteProb(0.0),
            edgeAddProbGlobal(0.1),
            edgeAddProbNewNode(0.1),
            edgeDeleteProb(0.1){};
        double nodeAddProb;
        double nodeDeleteProb;
        double edgeAddProbGlobal;
        double edgeAddProbNewNode;
        double edgeDeleteProb;
    };

    class Network
    {
    public:
        Network(int nrVertices)
            {
                Vertex *node;
                mVertices.resize(nrVertices);
                for (nrVertices--;nrVertices>=0;nrVertices--)
                {
                    node = new Vertex;
                    node->setIndex(nrVertices);
                    node->setRandomCo();
                    mVertices[nrVertices] = node;                
                }
            };
        ~Network()
            {
                int i;
                for (i=0;i<mVertices.size();i++)
                    delete mVertices[i];
                for (i=0;i<mEdges.size();i++)
                    delete mEdges[i];
            }
        bool addVertex()
            {
                Vertex *node = new Vertex;
                node->setIndex(mVertices.size());
                node->setRandomCo();
                mVertices.push_back(node);
                return true;
            }
        Network *duplicate()
            {
                Network *network = new Network(mVertices.size());
                int i;
                Edge *edge;
                for (i=0;i<mEdges.size();i++)
                {
                    edge = mEdges[i];
                    cout << edge->start()->index() << " " << edge->end()->index() << endl;
                    network->addEdge(edge->start()->index(),edge->end()->index());
                }
                return network;
            }
        bool addEdge(int six, int eix)
            {
                Edge *edge = new Edge(mVertices[six], mVertices[eix]);
                mEdges.push_back(edge);
                mVertices[six]->addEdge(mEdges.back());
                mVertices[eix]->addEdge(mEdges.back());
                return true;
            };
        bool removeVertex(long ix)
            {
                if (ix<0 || ix>=mVertices.size())
                {
                    cerr << "Vertex index is out of range, I cannot delete it" << endl;
                    return false;
                }
                int i;
                Vertex *node = mVertices[ix],*neighbour;
                Edge *edge;
                for (i=0;i<mEdges.size();)
                {
                    edge = mEdges[i];
                    if (edge->connects2(node))
                    {
                        neighbour = edge->otherSide(node);
                        node->removeEdge(edge);
                        neighbour->removeEdge(edge);
                        delete edge;
                        mEdges.erase(mEdges.begin()+i);
                    } else i++;
                }
                // Remove the vertex
                mVertices.erase(mVertices.begin()+ix);
                // Adjust the indices
                for (i=ix;i<mVertices.size();i++)
                    mVertices[i]->setIndex(i);
                return true;
            }
        long nrVertices(){return mVertices.size();};
        Vertex *getVertex(long i){return mVertices[i];};
        string toString()
            {
                ostringstream ostr;
                ostr << "This network has " << mVertices.size() << " vertices and " << mEdges.size() << " edges" << endl;
                int i,j;
                Vertex *self,*neighbour;
                for (i=0;i<mVertices.size();i++)
                    ostr << "\t" << mVertices[i]->toString() << endl;
                return ostr.str();
            }
        bool expand(long nrIter)
            {
                int i,j,nrV = nrVertices();
                long iter;
                Vertex *node1,*node2;
                double tmp,springDist;
                vector<double> co1,co2,coTmp,force;
                for (iter=0;iter<nrIter;iter++)
                {
                    // Clear the forces
                    for (i=0;i<nrV;i++)
                        mVertices[i]->clearForce();
                    // Add repel forces
                    for (i=0;i<nrV;i++)
                    {
                        node1 = mVertices[i];
                        node1->getCo(co1);
                        for (j=0;j<i;j++)
                        {
                            node2 = mVertices[j];
                            node2->getCo(co2);
                            coTmp = co2;
                            Vector::diff(coTmp,co1);
                            tmp = 1.0/Vector::l2Norm(coTmp);
                            Vector::multiply2All(coTmp,tmp);
                            node1->addForce(coTmp,true);
                            node2->addForce(coTmp,false);
                        }
                    }
                    // Add spring forces
                    for (i=0;i<nrV;i++)
                    {
                        node1 = mVertices[i];
                        node1->getCo(co1);
                        for (j=0;j<node1->nrNeighbours();j++)
                        {
                            node2 = node1->getNeighbour(j);
                            if (node1 < node2)
                            {
                                node2->getCo(co2);
                                coTmp = co2;
                                Vector::diff(coTmp,co1);
                                springDist = Vector::l2Norm(coTmp);
                                tmp = 1*(1-springDist);
                                Vector::multiply2All(coTmp,tmp);
                                node1->addForce(coTmp,true);
                                node2->addForce(coTmp,false);
                            }
                        }
                    }
                    // Adjust positions
                    for (i=0;i<nrV;i++)
                    {
                        node1 = mVertices[i];
                        node1->getCo(co1);
                        node1->getForce(force);
                        Vector::add(co1,force,0.01);
                        node1->setCo(co1);
                    }
                }
                // Normalize the coordinates of the nodes
                Vector::set(co2,0.0,0.0);
                for (i=0;i<nrV;i++)
                {
                    mVertices[i]->getCo(co1);
                    Vector::add(co2,co1,1.0/nrV);
                }
                for (i=0;i<nrV;i++)
                {
                    mVertices[i]->getCo(co1);
                    Vector::diff(co1,co2);
                    mVertices[i]->setCo(co1);
                }
            }
        bool mutate(MutationCfg mcfg)
            {
                // delete some nodes;
                int i;
                BernoulliDistribution bd(0);
                bool b;
                // Delete nodes
                bd.setProbTrue(mcfg.nodeDeleteProb/nrVertices());
                for (i=0;i<nrVertices();i++)
                {
                    if (bd.draw(b) && b)
                    {
                        cout << "Removing node " << i << endl;
                        removeVertex(i);
                    }
                }
                // Add nodes
                bd.setProbTrue(mcfg.nodeAddProb);
                bool mustAdd;
                int nrV = nrVertices();
                for (i=0;i<nrV;i++)
                {
                    if (bd.draw(b) && b)
                    {
                        vector<int> edges;
                        int j;
                        BernoulliDistribution bdH(mcfg.edgeAddProbNewNode);
                        for (j=0;j<nrV;j++)
                            if (bdH.draw(b) && b)
                                edges.push_back(j);
                        if (!edges.empty())
                        {
                            addVertex();
                            for (j=0;j<edges.size();j++)
                                addEdge(mVertices.size()-1,edges[j]);
                        }
                    }
                }
                return true;
            }
    private:
        vector<Vertex*> mVertices;
        vector<Edge*> mEdges;
    };
}

#endif
