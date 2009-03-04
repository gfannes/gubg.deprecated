#ifndef convexRegion_hpp
#define convexRegion_hpp

#include <iostream>
#include <vector>

#include "vector.hpp"
#include "block.hpp"
#include "grid.hpp"
#include "function.hpp"
#include "distribution.hpp"
#include "metropolis.hpp"
#include "kernel.hpp"
#include "serialize.hpp"

using namespace std;

class ConvexRegion
{
public:
    class Point
    {
    public:
        Point(double xx = 0.0, double yy = 0.0):
            x(xx),
            y(yy){}
        void set(double xx, double yy)
        {
            x = xx;
            y = yy;
        }
        bool operator!=(const Point &rhs)
            {
                return (x!=rhs.x) || (y!=rhs.y);
            }
    static Point* allocate4Serialize(){return (new Point);}
    bool serialize4Serialize(ostream &output)
            {
                return Serialize::serialize(x, output) && Serialize::serialize(y, output);
            }
    bool unserialize4Serialize(istream &input)
            {
                return Serialize::unserialize(x, input) && Serialize::unserialize(y, input);
            }

        double x;
        double y;
    };

    ConvexRegion():
        mValid(false){}

    bool learn(const vector<Point> &points)
        {
            if (points.size() < 2)
            {
                cerr << "I need at least two points to learn a convex region" << endl;
                return false;
            }

            // Collect information about the bounding box and setup the right, top, left and bottom points
            Point rightPoint  = points[0];
            Point topPoint    = points[0];
            Point leftPoint   = points[0];
            Point bottomPoint = points[0];
            mMinX = mMaxX = rightPoint.x;
            mMinY = mMaxY = rightPoint.y;
            for (int i=0; i<points.size(); ++i)
            {
                const Point &p = points[i];
                if (p.x < mMinX)
                {
                    mMinX = p.x;
                    leftPoint = p;
                }
                if (p.x > mMaxX)
                {
                    mMaxX = p.x;
                    rightPoint = p;
                }
                if (p.y < mMinY)
                {
                    mMinY = p.y;
                    bottomPoint = p;
                }
                if (p.y > mMaxY)
                {
                    mMaxY = p.y;
                    topPoint = p;
                }
            }

            // Put the points (no the corner points) in the different quadrants.
            vector<Point> q1, q2, q3, q4;
            for (int i=0; i<points.size(); ++i)
            {
                const Point &p = points[i];
                if (p.y>rightPoint.y && p.x>topPoint.x)
                    q1.push_back(p);
                if (p.y>leftPoint.y && p.x<topPoint.x)
                    q2.push_back(p);
                if (p.y<leftPoint.y && p.x<bottomPoint.x)
                    q3.push_back(p);
                if (p.y<rightPoint.y && p.x>bottomPoint.x)
                    q4.push_back(p);
            }
            
            mPoints.resize(0);
            mPoints.push_back(rightPoint);
            vector<double> prevDir, dir;

            double minAngle = 0.0, tmpAngle, cornerAngle;
            int minIX;
            vector<double> start, end;

            // Search and add Points from quadrant 1
            Vector::set(prevDir, 0.0, 1.0);
            while (mPoints.back() != topPoint)
            {
                Vector::set(start, mPoints.back().x, mPoints.back().y);
                // Compute the angle with the next corner node
                Vector::set(end, topPoint.x, topPoint.y);
                Vector::direction(dir, start, end);
                cornerAngle = minAngle = Vector::angle(dir, prevDir);
                // Find the node with smallest angle in the q-list
                for (int i=0; i<q1.size(); ++i)
                {
                    Vector::set(end, q1[i].x, q1[i].y);
                    Vector::direction(dir, start, end);
                    tmpAngle = Vector::angle(dir, prevDir);
                    if (i == 0 || tmpAngle < minAngle)
                    {
                        minAngle = tmpAngle;
                        minIX = i;
                    }
                }
                if (cornerAngle <= minAngle)
                    mPoints.push_back(topPoint); // corner point is next, we are finished
                else
                {
                    mPoints.push_back(q1[minIX]);
                    // Filter all points we can from the q-list
                    double level = mPoints.back().y;
                    int ix = 0;
                    for (int i=0; i<q1.size(); ++i)
                    {
                        Point &p = q1[i];
                        if (p.y > level)
                        {
                            if (i != ix)
                                q1[ix] = q1[i];
                            ++ix;
                        }
                    }
                    q1.resize(ix);
                }
            }

            // Search and add Points from quadrant 2
            Vector::set(prevDir, -1.0, 0.0);
            while (mPoints.back() != leftPoint)
            {
                Vector::set(start, mPoints.back().x, mPoints.back().y);
                // Compute the angle with the next corner node
                Vector::set(end, leftPoint.x, leftPoint.y);
                Vector::direction(dir, start, end);
                cornerAngle = minAngle = Vector::angle(dir, prevDir);
                // Find the node with smallest angle in the q-list
                for (int i=0; i<q2.size(); ++i)
                {
                    Vector::set(end, q2[i].x, q2[i].y);
                    Vector::direction(dir, start, end);
                    tmpAngle = Vector::angle(dir, prevDir);
                    if (i == 0 || tmpAngle < minAngle)
                    {
                        minAngle = tmpAngle;
                        minIX = i;
                    }
                }
                if (cornerAngle <= minAngle)
                    mPoints.push_back(leftPoint); // corner point is next, we are finished
                else
                {
                    mPoints.push_back(q2[minIX]);
                    // Filter all points we can from the q-list
                    double level = mPoints.back().x;
                    int ix = 0;
                    for (int i=0; i<q2.size(); ++i)
                    {
                        Point &p = q2[i];
                        if (p.x < level)
                        {
                            if (i != ix)
                                q2[ix] = q2[i];
                            ++ix;
                        }
                    }
                    q2.resize(ix);
                }
            }

            // Search and add Points from quadrant 3
            Vector::set(prevDir, 0.0, -1.0);
            while (mPoints.back() != bottomPoint)
            {
                Vector::set(start, mPoints.back().x, mPoints.back().y);
                // Compute the angle with the next corner node
                Vector::set(end, bottomPoint.x, bottomPoint.y);
                Vector::direction(dir, start, end);
                cornerAngle = minAngle = Vector::angle(dir, prevDir);
                // Find the node with smallest angle in the q-list
                for (int i=0; i<q3.size(); ++i)
                {
                    Vector::set(end, q3[i].x, q3[i].y);
                    Vector::direction(dir, start, end);
                    tmpAngle = Vector::angle(dir, prevDir);
                    if (i == 0 || tmpAngle < minAngle)
                    {
                        minAngle = tmpAngle;
                        minIX = i;
                    }
                }
                if (cornerAngle <= minAngle)
                    mPoints.push_back(bottomPoint); // corner point is next, we are finished
                else
                {
                    mPoints.push_back(q3[minIX]);
                    // Filter all points we can from the q-list
                    double level = mPoints.back().y;
                    int ix = 0;
                    for (int i=0; i<q3.size(); ++i)
                    {
                        Point &p = q3[i];
                        if (p.y < level)
                        {
                            if (i != ix)
                                q3[ix] = q3[i];
                            ++ix;
                        }
                    }
                    q3.resize(ix);
                }
            }

            // Search and add Points from quadrant 4
            Vector::set(prevDir, 1.0, 0.0);
            while (mPoints.back() != rightPoint)
            {
                Vector::set(start, mPoints.back().x, mPoints.back().y);
                // Compute the angle with the next corner node
                Vector::set(end, rightPoint.x, rightPoint.y);
                Vector::direction(dir, start, end);
                cornerAngle = minAngle = Vector::angle(dir, prevDir);
                // Find the node with smallest angle in the q-list
                for (int i=0; i<q4.size(); ++i)
                {
                    Vector::set(end, q4[i].x, q4[i].y);
                    Vector::direction(dir, start, end);
                    tmpAngle = Vector::angle(dir, prevDir);
                    if (i == 0 || tmpAngle < minAngle)
                    {
                        minAngle = tmpAngle;
                        minIX = i;
                    }
                }
                if (cornerAngle <= minAngle)
                    mPoints.push_back(rightPoint); // corner point is next, we are finished
                else
                {
                    mPoints.push_back(q4[minIX]);
                    // Filter all points we can from the q-list
                    double level = mPoints.back().x;
                    int ix = 0;
                    for (int i=0; i<q4.size(); ++i)
                    {
                        Point &p = q4[i];
                        if (p.x > level)
                        {
                            if (i != ix)
                                q4[ix] = q4[i];
                            ++ix;
                        }
                    }
                    q4.resize(ix);
                }
            }

            // Remove the last Point (rightPoint) because we have this twice
            mPoints.pop_back();

            mValid = true;
            return true;
        }

    template <typename FunctionT>
    bool learn(FunctionT &function, int nrPoints, int nrIter = 100, double proposalSigma = 0.03, double initFrac = 0.1, double x = 0, double y = 0, double w = 1, double h = 1, int nrX = 20, int nrY = 20)
        {
            // Generate the initial values as the start position
            vector<Point> points;
            for (int i=0; i<nrPoints; ++i)
            {
                Point p(Random::drawUniform(x+initFrac*w, x+(1-initFrac)*w), Random::drawUniform(y+initFrac*h, y+(1-initFrac)*h));
                points.push_back(p);
            }
            learn(points);

            // Add the locations of a grid to "points", which will be used as input points for the distribution difference computation
            points.resize(0);
            vector<double> outputs;
            Grid grid(x, y, w, h, nrX, nrY, false);
            BlockH<FunctionT> block(points, outputs, function);
            grid.each(block);
            
            L1Dist l1Dist(points, outputs, nrX*nrY/5);
            Proposal proposal(nrPoints, x, y, w, h, proposalSigma);
            
            Metropolis<L1Dist, Proposal> metro(l1Dist, proposal, true);
            metro.run(*this, nrIter, true);

            return true;
        }

    bool filter(vector<Point> &internal, vector<Point> &external, const vector<Point> &source)
        {
            internal.resize(0);
            external.resize(0);
            for (int i=0; i<source.size(); ++i)
            {
                const Point &p = source[i];
                if (isInternal(p))
                    internal.push_back(p);
                else
                    external.push_back(p);
            }
            return true;
        }

    bool isInternal(const Point &p)
        {
            bool isInt = true;
            if (p.x<mMinX || p.x>mMaxX || p.y<mMinY || p.y>mMaxY)
                isInt = false;
            for (int j=0; isInt && j<mPoints.size(); ++j)
                isInt = onTheLeftSide(p.x, p.y, j);
            return isInt;
        }

    template <typename BlockT>
    bool eachSegment(BlockT &block)
        {
            if (!mValid)
            {
                cerr << "The region is not valid, please learn it first" << endl;
                return false;
            }
            block.preYield();
            int i;
            for (i=1; i<mPoints.size(); ++i)
                block.yield(mPoints[i-1], mPoints[i]);
            block.yield(mPoints[i-1], mPoints[0]);
            block.postYield();
            return true;
        }
    long nrSegments()
        {
            if (!mValid)
            {
                cerr << "The convex region is not valid, I cannot get the number of segments" << endl;
                return -1;
            }
            return mPoints.size();
        }
    bool getSegment(Point &start, Point &end, long ix)
        {
            if (!mValid)
            {
                cerr << "The region is not valid, please learn it first" << endl;
                return false;
            }
            if (ix<0 || ix>=mPoints.size())
            {
                cerr << "IX is out of range, I cannot get segment " << ix << "" << endl;
                return false;
            }
            start = mPoints[ix];
            if (ix<mPoints.size()-1)
                end = mPoints[ix+1];
            else
                end = mPoints[0];
            return true;
        }

    const vector<Point> &getPoints(){return mPoints;}

    void print()
        {
            cout << "Convex region consists of " << mPoints.size() << " nodes" << endl;
            for (int i=0; i<mPoints.size(); ++i)
            {
                cout << "Node " << i << " = (" << mPoints[i].x << ", " << mPoints[i].y << ")" << endl;
            }
        }

    // Serialization methods
    static ConvexRegion* allocate4Serialize(){return (new ConvexRegion);}
    bool serialize4Serialize(ostream &output)
        {
            return Serialize::serialize(mPoints, output);
        }
    bool unserialize4Serialize(istream &input)
        {
            vector<Point> points;
            Serialize::unserialize(points, input);
            return learn(points);
        }
    
private:
    template <typename FunctionT>
    class BlockH: public Block<Grid>
    {
    public:
        BlockH(vector<Point> &points, vector<double> &outputs, FunctionT &function):
            mPoints(points),
            mOutputs(outputs),
            mFunction(&function){}
        bool yield(double x, double y)
            {
                Point p(x, y);
                mPoints.push_back(p);
                vector<double> inputs;
                Vector::set(inputs, x, y);
                double output = mFunction->output(inputs);
                mOutputs.push_back(output);
                return true;
            }
    private:
        vector<Point> &mPoints;
        vector<double> &mOutputs;
        FunctionT *mFunction;
    };

    class L1Dist: public Distribution<ConvexRegion>
    {
    public:
        L1Dist(vector<Point> points, vector<double> outputs, int minCount):
            mPoints(points),
            mOutputs(outputs),
            mMinCount(minCount){}
        double logDensity(ConvexRegion &cr)
            {
                vector<double> internal, external;
                for (int i=0; i<mPoints.size(); ++i)
                {
                    if (cr.isInternal(mPoints[i]))
                        internal.push_back(mOutputs[i]);
                    else
                        external.push_back(mOutputs[i]);
                }
                double distance = 0.0;
                if (internal.size()>=mMinCount && external.size()>=mMinCount)
                {
                    mInternal.learn(internal);
                    mExternal.learn(external);
                    distance =  mInternal.l1Distance(mExternal, 80);
                } else
                    cout << "MESSAGE::Not enough elements in internal or external while learning the ConvexRegion" << endl;
                double v = exp(distance*30);
//                cout << "v = " << v << "" << endl;
                return v;
            }
    private:
        KernelD<double> mExternal;
        KernelD<double> mInternal;
        vector<Point> mPoints;
        vector<double> mOutputs;
        int mMinCount;
    };

    class Proposal: public ConditionalDistribution<ConvexRegion, ConvexRegion>
    {
    public:
        Proposal(int nrPoints, double x, double y, double w, double h, double sigma):
            mNrPoints(nrPoints),
            mX(x),
            mY(y),
            mW(w),
            mH(h),
            mSigma(sigma){}
        bool draw(ConvexRegion &value, ConvexRegion &condition)
            {
                // Copy the condition-points
                vector<Point> points = condition.getPoints();
                // Add the points that are too few
                for (int i=points.size(); i<mNrPoints; ++i)
                {
                    Point start, end;
                    condition.getSegment(start, end, Random::drawUniformDiscrete(0, condition.nrSegments()));
                    double frac = Random::drawUniform();
                    start.x += frac*(end.x - start.x);
                    start.y += frac*(end.y - start.y);
                    points.push_back(start);
                }
                // Select a point and perturbate
                int ix = Random::drawUniformDiscrete(0, mNrPoints);
//                cout << "I choose point " << ix << "" << endl;
                Point &p = points[ix];
                double x = p.x, y = p.y;
                do
                {
                    p.x = Random::drawGaussian(x, mSigma);
                } while (p.x<mX || p.x>mX+mW);
                do
                {
                    p.y = Random::drawGaussian(y, mSigma);
                } while (p.y<mY || p.y>mY+mH);
//                cout << "p = " << p.x << ", " << p.y << ", " << x << ", " << y << "" << endl;
                // Learn the region
                value.learn(points);
                return false;
            }
    private:
        int mNrPoints;
        double mX;
        double mY;
        double mW;
        double mH;
        double mSigma;
    };

    bool onTheLeftSide(double x, double y, int segIX)
        {
            Point &start = mPoints[segIX];
            if (segIX == mPoints.size()-1)
            {
                Point &end = mPoints[0];
                return ((x-start.x)*(end.y-start.y) + (y-start.y)*(start.x-end.x)) <= 0.0;
            } else
            {
                Point &end = mPoints[segIX+1];
                return ((x-start.x)*(end.y-start.y) + (y-start.y)*(start.x-end.x)) <= 0.0;
            }
        }

    bool mValid;
    vector<Point> mPoints;
    double mMinX;
    double mMaxX;
    double mMinY;
    double mMaxY;
};

namespace Serialize
{
    SetType(ConvexRegion::Point,eTypeIDCRPoint);
    SetType(ConvexRegion,eTypeIDConvexRegion);
    SetTypeAndFunctions(vector<ConvexRegion::Point>,eTypeIDVectorCRPoint,BasicMethodsVector<ConvexRegion::Point>::allocate,BasicMethodsVector<ConvexRegion::Point>::serialize,BasicMethodsVector<ConvexRegion::Point>::unserialize);
    SetTypeAndFunctions(vector<ConvexRegion>,eTypeIDVectorConvexRegion,BasicMethodsVector<ConvexRegion>::allocate,BasicMethodsVector<ConvexRegion>::serialize,BasicMethodsVector<ConvexRegion>::unserialize);
};

#endif
