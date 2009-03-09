#include <iostream>

#include <vector>

#include "physics.hpp"
#include "physicsVisu.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  const int dimension = 2;
  Physics<dimension> physics;
  Physics<dimension>::NodeIter iterN;
  Physics<dimension>::NodeT *node, *prevNode = NULL, *tmpNode;
  vector<Physics<dimension>::NodeT*> nodes;

//     int i,j;
//     for (i=0;i<10;i++)
//     {
//         node = physics.addNode();
//         nodes.push_back(node);
//         for (j=0;j<i/2;j++)
//             physics.addLink(nodes[j], nodes[i]);
//     }
//     for (iterN = physics.initNodeIter() ; physics.getNode(node,iterN) ; iterN++)
//     {
//         cout << node << endl;
//     }
//     cout << "" << physics.toString() << "" << endl;

  node = physics.addNode();
  prevNode = physics.addNode();
  physics.addLink(node,prevNode);
  tmpNode = physics.addNode();
  physics.addLink(node,tmpNode);
  physics.addLink(prevNode,tmpNode);
  node = physics.addNode();
  physics.addLink(node,tmpNode);

  node = physics.addNode();
  Vector::set(node->location(),0.0,0.0);
  Vector::set(node->speed(),0.0,0.0);
  node->setCharge(0.0);

  Force<2> *force = new Force<2>;
  physics.addForce(force);

  int r = 15;
  PhysicsVisu physicsVisu(700,700,-r,-r,r,r,&physics);
  physicsVisu.start();
  return 0;
}
