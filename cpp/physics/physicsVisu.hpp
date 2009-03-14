#ifndef physicsVisu_h
#define physicsVisu_h

#include "physics.hpp"
#include "visualization.hpp"
#include "vector.hpp"

using namespace std;
using namespace gubg;
using namespace gubg::Vector;

class PhysicsVisu: public Visualization
{
public:
    typedef Physics<2>::NodeT NodeT;
    typedef Physics<2>::NodeIter NodeIter;
    typedef Physics<2>::LinkT LinkT;
    typedef Physics<2>::LinkIter LinkIter;

    PhysicsVisu(int xW, int yW, double minX, double minY, double maxX, double maxY, Physics<2> *physics):
        Visualization(xW,yW),
        mPhysics(physics)
        {
            vector<double> co,oco;
            NodeT *node, *otherNode;
            NodeIter ni;
            LinkT *link;
            LinkIter li;
            Circle *circle;
            Line *line;
            int basis;

            // Allocate the circles and lines
            double xO,yO,xU,yU;
            xO = -(xW-1)*minX/(maxX-minX);
            yO = -(yW-1)*minY/(maxY-minY);
            xU = (xW-1)/(maxX-minX);
            yU = (yW-1)/(maxY-minY);
            // cout << "xO = " << xO << ", yO = " << yO << ", xU = " << xU << ", yU = " << yU << "" << endl; // do(0)
            basis=createBasis(xO,yO,xU,yU);
            for (ni = mPhysics->initNodeIter() ; mPhysics->getNode(node,ni) ; ni++)
            {
                co = node->location();
                circle = new Circle(co[0], co[1], node->radius(), node->mass()*5);
                mCircles.push_back(circle);
                addDrawable(basis,circle);
                for (li = node->initLinkIter() ; node->getLinked(otherNode,li) ; li++)
                {
                    oco = otherNode->location();
                    line = new Line(co[0],co[1],oco[0],oco[1]);
                    mLines.push_back(line);
                    addDrawable(basis,line);
                }
            }
        };
private:
    virtual bool onStart()
        {
            // Set the time into the physics world
            // cout << "Setting time to " << mCurrentTick << "" << endl; // do(0)
            mPhysics->initializeSimulation(mTotalElapsedTime);
            return true;
        }
    virtual bool dynamics()
        {
            int nodeIX = 0,linkIX = 0;
            vector<double> co,oco;
            NodeT *node, *otherNode;
            NodeIter ni;
            LinkT *link;
            LinkIter li;

            // Do the physics simulation upto our current time (measured in ticks)
            mPhysics->simulateFor(mElapsedTime, mTotalElapsedTime);

            for (ni = mPhysics->initNodeIter() ; mPhysics->getNode(node,ni) ; ni++, nodeIX++)
            {
                co = node->location();
                mCircles[nodeIX]->setCentre(co[0],co[1]);
                // draw the edges
                for (li = node->initLinkIter() ; node->getLinked(otherNode,li) ; li++, linkIX++)
                {
                    oco = otherNode->location();
                    mLines[linkIX]->setCoordinates(co[0],co[1],oco[0],oco[1]);
                }
            }
            return true;
        }

    Random mRandom;
    Physics<2> *mPhysics;
    vector<Circle*> mCircles;
    vector<Line*> mLines;
};

// int main()
// {
//     Network netw(5),*network,*network2;
//     int i,j,k;
//     netw.addEdge(0,1);
//     netw.addEdge(1,2);
//     netw.addEdge(4,2);
//     netw.addEdge(0,2);
//     netw.addEdge(0,3);
//     netw.addEdge(0,4);
//     cout << netw.toString() << endl;

//     int basis;
//     Line *line;
//     Circle *circle;
//     PhysicsVisu visu(1200,600);
// //    visu.networks.push_back(&netw);
//     MutationCfg mcfg;
//     network2 = netw.duplicate();
//     network2->mutate(mcfg);
//     visu.networks.push_back(network2);
//     vector<double> co,coE;
//     Vertex *node;

//     basis=visu.createBasis(600,300,100,100);
//     for (k=0;k<visu.networks.size();k++)
//     {
//         network = visu.networks[k];
//         for (i=0;i<network->nrVertices();i++)
//         {
//             node = network->getVertex(i);
//             node->getCo(co);
//             // draw this node
//             circle=new Circle(co[0],co[1],0.2,0.01);
//             visu.circles.push_back(circle);
//             visu.addDrawable(basis,circle);
//             // draw the edges
//             for (j=0;j<node->nrNeighbours();j++)
//             {
//                 node->getNeighbour(j)->getCo(coE);
//                 line=new Line(co[0],co[1],coE[0],coE[1],0.01);
//                 visu.lines.push_back(line);
//                 visu.addDrawable(basis,line);
//             }
//         }
//     }
//     visu.start();
//     return 0;
// }

#endif
