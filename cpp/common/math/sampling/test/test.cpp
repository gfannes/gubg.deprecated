#include "metropolis.hpp"
#include "vector.hpp"
#include "gaussian.hpp"
#include "datavisu.hpp"

// Has a minimum of 0 at (1,1)
double rosenBrock(double x, double y)
{
  return (1-x)*(1-x) + 100*(y-x*x)*(y-x*x);
}

class RosenBrockD: public Distribution<vector<double> >
  {
  public:
    double logDensity(vector<double> &loc)
    {
      return -rosenBrock(loc[0],loc[1]);
    }
  };

int main(int argc, char *argv[])
{
  RosenBrockD rb;
  vector<double> sigma;
  Vector::set(sigma,0.01,0.01);
  GaussianCD<vector<double> > cgd(sigma);
  Metropolis<RosenBrockD,GaussianCD<vector<double> > > metropolis(rb, cgd);
  metropolis.run(sigma,100000);

  vector<vector<double> > locations;
  locations = metropolis.locations();
  vector<double> xs,ys;
  for (int i=0;i<locations.size();++i)
    {
      xs.push_back(locations[i][0]);
      ys.push_back(locations[i][1]);
    }

  DataVisu datavisu(1200,600);
  datavisu.addLines(xs,ys);
//     datavisu.addRectangle(0,0,1,1);
//     datavisu.addRectangle(0.2,0.2,0.4,0.4);
  datavisu.show();

  return 0;
}
