#include <iostream>

#include "vertex.hpp"
#include "edge.hpp"
#include "network.hpp"
#include "visualization.hpp"
#include "vector.hpp"

using namespace std;
using namespace Vector;

class VisualizationH: public Visualization
  {
  public:
    VisualizationH(int xW, int yW):Visualization(xW,yW) {};
    vector<Circle*> circles;
    vector<Line*> lines;
    vector<Network*> networks;
  private:
    virtual bool dynamics()
    {
      int i,j,k,vertexIX = 0,edgeIX = 0;
      vector<double> co,coE;
      Vertex *node;
      Network *network;

      for (k=0;k<networks.size();k++)
        {
          network = networks[k];
          network->expand(1);

          for (i=0;i<network->nrVertices();i++)
            {
              node = network->getVertex(i);
              node->getCo(co);
              circles[vertexIX++]->setCentre(co[0],co[1]);
              // draw the edges
              for (j=0;j<node->nrNeighbours();j++)
                {
                  node->getNeighbour(j)->getCo(coE);
                  lines[edgeIX++]->setCoordinates(co[0],co[1],coE[0],coE[1]);
                }
            }
        }

      return true;
    }
    Random mRandom;
  };

int main(int argc, char *argv[])
{
  Network netw(5),*network,*network2;
  int i,j,k;
  netw.addEdge(0,1);
  netw.addEdge(1,2);
  netw.addEdge(4,2);
  netw.addEdge(0,2);
  netw.addEdge(0,3);
  netw.addEdge(0,4);
  cout << netw.toString() << endl;

  int basis;
  Line *line;
  Circle *circle;
  VisualizationH visu(1200,600);
//    visu.networks.push_back(&netw);
  MutationCfg mcfg;
  network2 = netw.duplicate();
  network2->mutate(mcfg);
  visu.networks.push_back(network2);
  vector<double> co,coE;
  Vertex *node;

  basis=visu.createBasis(600,300,100,100);
  for (k=0;k<visu.networks.size();k++)
    {
      network = visu.networks[k];
      for (i=0;i<network->nrVertices();i++)
        {
          node = network->getVertex(i);
          node->getCo(co);
          // draw this node
          circle=new Circle(co[0],co[1],0.2);
          visu.circles.push_back(circle);
          visu.addDrawable(basis,circle);
          // draw the edges
          for (j=0;j<node->nrNeighbours();j++)
            {
              node->getNeighbour(j)->getCo(coE);
              line=new Line(co[0],co[1],coE[0],coE[1]);
              visu.lines.push_back(line);
              visu.addDrawable(basis,line);
            }
        }
    }
  visu.start();
  return 0;
}
