#ifndef HEADER_gubg_physics_link_hpp_ALREADY_INCLUDED
#define HEADER_gubg_physics_link_hpp_ALREADY_INCLUDED

using namespace std;

template <int dim>
class Node;

template <int dim>
class Link
{
public:
    const static int dimension = dim;
    typedef Node<dim> NodeT;

    Link(NodeT *start, NodeT *end):
        mStart(start),
        mEnd(end),
        mStretchSpringLength(5.0),
        mShrinkSpringLength(5.0),
        mStretchSpringK(100.0),
        mShrinkSpringK(100.0),
        mStretchDamp(10.0),
        mShrinkDamp(10.0)
        {};
    NodeT *otherSide(NodeT *thisSide){return ((thisSide==mStart) ? mEnd : mStart);};

    void initialize()
        {
            vector<double> distanceVector = mEnd->location();
            Vector::diff(distanceVector,mStart->location());
            mPrevLength = Vector::l2Norm(distanceVector);
            mPrevTimeStep = 0;
        }    

    bool computeSpringDampForce(vector<double> &force, double timeStep)
        {
            // Compute the spring force
            vector<double> direction;
            direction = mEnd->location();
            Vector::diff(direction,mStart->location());
            double springLength = Vector::l2Norm(direction);
            if (springLength < 0.000000001)
                cerr << "WARNING::Two nodes (" << mStart << ", " << mEnd << ") are rather close: " << springLength << "" << endl;
            double forceFactor;
            if (springLength < mShrinkSpringLength)
                forceFactor = mShrinkSpringK*(mShrinkSpringLength - springLength)/springLength;
            else if (springLength > mStretchSpringLength)
                forceFactor = mStretchSpringK*(mStretchSpringLength - springLength)/springLength;
            force = direction;
            Vector::multiply2All(force,forceFactor);
            // Compute the damping force
            if (mPrevTimeStep > 0)
            {
                double speed = (springLength - mPrevLength)/mPrevTimeStep;
                if (speed > 0)
                    forceFactor = -mStretchDamp*speed/springLength;
                else
                    forceFactor = -mShrinkDamp*speed/springLength;
                Vector::multiply2All(direction,forceFactor);
                Vector::add(force,direction);
            }
            mPrevLength = springLength;
            mPrevTimeStep = timeStep;
            return true;
        }            

    NodeT *start(){return mStart;};
    NodeT *end(){return mEnd;};
//     bool links2(NodeType<dimension> *someSide){return (someSide==mStart)||(someSide==mEnd);};
//     string toString()
//         {
//             ostringstream ostr;
//             ostr << "(Link) this = " << this << " mStart = " << mStart << " mEnd = " << mEnd;
//             return ostr.str();
//         }
private:
    NodeT *mStart;
    NodeT *mEnd;

    double mStretchSpringLength; // If the link is larger than this length, it is considered "streched", and you should apply a spring force inwards with constant mStretchSpringK
    double mShrinkSpringLength;
    double mStretchSpringK;
    double mShrinkSpringK;
    double mStretchDamp;
    double mShrinkDamp;
    double mPrevLength;
    double mPrevTimeStep;
};

#endif
