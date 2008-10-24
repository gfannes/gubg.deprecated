#ifndef node_h
#define node_h

#include <list>
#include <vector>
#include <sstream>

#include "vector.hpp"
#include "utils.hpp"

using namespace std;

template <int dimension>
class NodeAttr
{
public:
    NodeAttr(double mass = 0.5, double radius = 1, double bounceFrac = 1, double charge = 10, double damp = 0.3, vector<double> *location = NULL, vector<double> *speed = NULL):
        mMass(mass),
        mRadius(radius),
        mBounceFrac(bounceFrac),
        mCharge(charge),
        mDamp(damp)
        {
            if (location && location->size()==dimension)
                mLocation = *location;
            else
            {
                if (location)
                    cerr << "The dimension of the provided location vector does not correspond with the dimension of the node" << endl;
                Vector::setUniform(mLocation,dimension,-0,20);
            }
            if (speed && speed->size()==dimension)
                mSpeed = *speed;
            else
            {
                if (speed)
                    cerr << "The dimension of the provided speed vector does not correspond with the dimension of the node" << endl;
//                Vector::setUniform(mSpeed,dimension,-1,1);
                Vector::setUniform(mSpeed,dimension,-0,0);
            }
        }

    double mass(){return mMass;};
    double radius(){return mRadius;};
    double bounceFrac(){return mBounceFrac;};
    double charge(){return mCharge;};
    void setCharge(double charge){mCharge = charge;};
    vector<double> &location(){return mLocation;};
    vector<double> &speed(){return mSpeed;};
protected:
    double mMass;
    double mRadius;
    double mBounceFrac;
    double mCharge;
    double mDamp;
    vector<double> mLocation;
    vector<double> mSpeed;
};

template <int dim>
class Link;

template <int dim>
class Node: public NodeAttr<dim>
{
public:
    const static int dimension = dim;
    typedef Link<dim> LinkT;
    typedef typename list<LinkT*>::iterator LinkIter;
    typedef Node<dim> NodeT;
    typedef NodeAttr<dim> NodeAttrT;

    Node():NodeAttr<dim>(){}
    Node(NodeAttrT *attr):NodeAttr<dim>(*attr){}

    // Adding a link
    bool addLink(LinkT *link){mLinks.push_back(link);return true;};

    // Iteration over the links (either links themselves, or the linked nodes)
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
    bool getLinked(NodeT *&node, LinkIter iter)
        {
            if (iter!=mLinks.end())
            {
                node = (*iter)->otherSide(this);
                return true;
            }
            return false;
        }

    bool clearForce(){return Vector::setSame(mForce,dimension,0.0);}
    bool collectSpringDampForces(double timeStep)
        {
            LinkIter li;
            LinkT *link;
            vector<double> forcePart;
            // Collect the spring-damp forces for nodes that are linked to here
            for (li = initLinkIter() ; getLink(link,li) ; li++)
            {
                if (this == link->start())
                {
                    link->computeSpringDampForce(forcePart,timeStep);
                    Vector::add(mForce,forcePart,-1.0);
                    Vector::add(link->end()->mForce,forcePart,1.0);
                }
            }
            return true;
        }

    bool addDampForce()
        {
            return Vector::add(mForce,this->mSpeed,-this->mDamp);
        }

    bool updateSpeed(double timeStep)
        {
            return Vector::add(this->mSpeed, mForce, timeStep/this->mMass);
        }

    bool updateLocation(double timeStep)
        {
            return Vector::add(this->mLocation, this->mSpeed, timeStep);
        }

    bool addElectrostaticForce(NodeT *onode)
        {
            vector<double> force = onode->mLocation;
            Vector::diff(force,this->mLocation);
            double distance = Vector::l2Norm(force);
            double forceFactor = 1.0/distance;
            Vector::multiply2All(force,forceFactor);
            if (distance < this->mRadius + onode->radius())
                distance = this->mRadius + onode->radius();
            forceFactor = this->charge()*onode->charge()/distance/distance;
            Vector::multiply2All(force,forceFactor);
//             cout << "force elect = " << Vector::toString(force) << "" << endl;
//             cout << "force       = " << Vector::toString(mForce) << "" << endl;
            Vector::add(mForce,force,-1.0);
            Vector::add(onode->mForce,force,1.0);
            return true;
        }            
    bool addForce(vector<double> &force, double factor = 1.0)
        {
            Vector::add(mForce,force,factor);
            return true;
        }

    string toString(int level = 0, bool showLinkedNodes = true)
        {
            ostringstream ostr;
            ostr << "" << indent(level) << "Node(" << dimension << ") " << this << ", (" << Vector::toString(this->mLocation) << "):" << endl;
            if (showLinkedNodes)
            {
                LinkIter li;
                NodeT *node;
                for (li = initLinkIter() ; getLinked(node,li) ; li++)
                    ostr << node->toString(level+1, false);
            }
            return ostr.str();
        }

//     bool removeLink(LinkT *link)
//         {
//             list<LinkT*>::iterator iterL;
//             for (iterL = mLinks.begin();iterL!=mLinks.end();iterL++)
//                 if (*iterL == link)
//                 {
//                     mLinks.erase(iterL);
//                     return true;
//                 }
//             return false;
//         }

//     void setCo(vector<double> location){mLocation = location;};
//     void setRandomCo(){Vector::setUniform(mLocation,2,-1,1);};
//     void getCo(vector<double> &location){location = mLocation;};

//     void clearForce(){Vector::set(mForce,0.0,0.0);};
//     void addForce(vector<double> &force, bool reverse){Vector::add(mForce,force,(reverse ? -1 : 1));};
//     void getForce(vector<double> &force){force = mForce;};

private:
    list<LinkT*> mLinks;
    vector<double> mForce;
};
#endif
