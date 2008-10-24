#ifndef physics_h
#define physics_h

#include <sstream>

#include "node.hpp"
#include "link.hpp"
#include "force.hpp"

template <int dim>
class Physics
{
public:
    const static int dimension = dim;
    typedef Node<dim> NodeT;
    typedef typename list<NodeT*>::iterator NodeIter;
    typedef Link<dim> LinkT;
    typedef typename list<LinkT*>::iterator LinkIter;
    typedef NodeAttr<dim> NodeAttrT;
    typedef Force<dim> ForceT;
    typedef typename list<ForceT*>::iterator ForceIter;

    Physics(double maxTimeD = 0.01, double maxSpaceD = 0.01):
        mMaxTimeD(maxTimeD),
        mMaxSpaceD(maxSpaceD){}
    ~Physics()
        {
            NodeIter iterN;
            NodeT *node;
            for (iterN = initNodeIter() ; getNode(node,iterN) ; iterN++)
                delete node;
            LinkIter iterL;
            LinkT *link;
            for (iterL = initLinkIter() ; getLink(link,iterL) ; iterL++)
                delete link;
        }

    // Adding forces
    bool addForce(ForceT *force)
        {
            mForces.push_back(force);
            return true;
        }

    // Adding nodes and links
    NodeT * addNode(NodeAttrT *attr = NULL)
        {
            NodeT *node = (attr ? new NodeT(attr) : new NodeT);            
            if (node)
                mNodes.push_back(node);
            return node;
        }
    LinkT * addLink(NodeT *start, NodeT *end)
        {
            LinkT *link = new LinkT(start, end);
            if (link)
            {
                mLinks.push_back(link);
                start->addLink(link);
                end->addLink(link);
            }
            return link;
        }

    // Iteration over nodes and links
    NodeIter initNodeIter(){return mNodes.begin();}
    bool getNode(NodeT *&node, NodeIter iter)
        {
            if (iter!=mNodes.end())
            {
                node = *iter;
                return true;
            }
            return false;
        }
    LinkIter initLinkIter(){return mLinks.begin();}
    bool getLink(LinkT *&link, LinkIter iter)
        {
            if (iter!=mLinks.end())
            {
                link = *iter;
                return true;
            }
            return false;
        }
    ForceIter initForceIter(){return mForces.begin();}
    bool getForce(ForceT *&force, ForceIter iter)
        {
            if (iter!=mForces.end())
            {
                force = *iter;
                return true;
            }
            return false;
        }

    // Simulate the physics upto a certain time
    void initializeSimulation(long time)
        {
            mTime = time;
            LinkT *link;
            LinkIter li;
            for (li = initLinkIter() ; getLink(link,li) ; li++)
                link->initialize();
        };
    bool simulateFor(long elapsedTime, long totalElapsedTime)
        {
            double timeStep = (elapsedTime) / 1000000.0;
            NodeT *node,*onode;
            NodeIter ni,ni2;
            LinkT *link;
            LinkIter li;
            ForceT *force;
            ForceIter fi;
            vector<double> forcePart;
            // Clear all the forces on the nodes
            for (ni = initNodeIter() ; getNode(node,ni) ; ni++)
                node->clearForce();
            for (fi = initForceIter(); getForce(force,fi); fi++)
                force->addForce(mNodes);
            for (ni = initNodeIter() ; getNode(node,ni) ; ni++)
            {
                // cout << "Time step = " << timeStep << "" << endl; // do(0)

                // Collect all forces that are working on this node
                // Collect the electrostatic forces
                node->collectSpringDampForces(timeStep);
                for (ni2 = initNodeIter() ; getNode(onode,ni2) ; ni2++)
                    if (node < onode)
                        node->addElectrostaticForce(onode);
                node->addDampForce();
            }

            for (ni = initNodeIter() ; getNode(node,ni) ; ni++)
            {
                // Update the speed according to newton
                node->updateSpeed(timeStep);

                // Update the location according to its speed
                node->updateLocation(timeStep);
            }
            mTime = totalElapsedTime;
            return true;
        }

    string toString(int level = 0)
        {
            ostringstream ostr;
            ostr << "" << indent(level) << "Physics (" << this << ") (max time delta = " << mMaxTimeD << ", max space delta = " << mMaxSpaceD << ")" << endl;
            ostr << "" << indent(level+1) << "nr nodes = " << mNodes.size() << ", nr links = " << mLinks.size() << "" << endl;
            NodeIter ni;
            NodeT *node;
            for (ni=initNodeIter() ; getNode(node,ni) ; ni++)
            {
                ostr << node->toString(level+1);
            }
            return ostr.str();
        }
private:
    // Add some group concept
    list<NodeT*> mNodes;
    list<LinkT*> mLinks;
    list<ForceT*> mForces;

    double mMaxTimeD;
    double mMaxSpaceD;

    long mTime;
};

#endif
