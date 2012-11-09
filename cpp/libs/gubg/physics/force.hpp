#ifndef HEADER_gubg_physics_force_hpp_ALREADY_INCLUDED
#define HEADER_gubg_physics_force_hpp_ALREADY_INCLUDED

#include <iostream>

#include "node.hpp"
#include "vector.hpp"
#include "math.hpp"

using namespace std;

template <int dim>
class Force
{
public:
    typedef Node<dim> NodeT;
    typedef typename list<NodeT*>::iterator NodeIter;
    typedef Link<dim> LinkT;
    typedef typename list<LinkT*>::iterator LinkIter;

    bool addForce(list<NodeT*> nodes)
        {
//             cout << "addForce" << endl;
            NodeT *node0,*node1;
            NodeIter iter = nodes.begin();
            node0 = *iter;
            iter++;
            node1 = *iter;
            vector<double> t,mid;
            mid = node0->location();
            Vector::add(mid,node1->location());
            Vector::multiply2All(mid,0.5);
            double dist = Vector::l2Norm(mid);
            cout << "Distance = " << dist << "" << endl;
            tangent(t,node0->location(),node1->location());
            double angle = Vector::angle(t,mid);
            cout << "Angle = " << angle << "" << endl;

            if (angle<Math::PI)
            {
                cout << "Rechter oor" << endl;
                node0->addForce(t,10);
            }
            if (angle>Math::PI)
            {
                cout << "Linker oor" << endl;
                node1->addForce(t,10);
            }
            return true;
        }
};

#endif
