#ifndef sequence_h
#define sequence_h

#include <vector>

using namespace std;

template<typename T>
class Sequence
{
public:
    class EachBlock
    {
    public:
        bool preYield(){return true;};
        bool yield(T v){return true;};
        bool postYield(){return true;};

        int index;
        T value;
        T begin;
        T end;
        T increment;
        Sequence<T> *pSequence;
    };
    template<typename TResult>
    class CollectBlock
    {
    public:
        bool preYield(){return true;};
        bool yield(TResult &res,T v){return true;};
        bool postYield(){return true;};

        typedef TResult ResultT;
        int index;
        T value;
        T begin;
        T end;
        T increment;
        Sequence<T> *pSequence;
    };
    Sequence(T end)
        {
            mBegin=0;
            mEnd=end-1;
            mIncrement=1;
        }
    Sequence(T begin, T end, T increment=1);

    template<typename Block>
    bool each(Block &block)
        {
            T value;
            block.pSequence = this;
            block.begin=mBegin;
            block.end=mEnd;
            block.increment=mIncrement;
            block.index=0;
            block.value=mBegin;

            // Pre
            if (!block.preYield())
            {
                cerr << "preYield failed for Sequence#each, make sure preYield returns true" << endl;
                return false;
            }

            if (mIncrement>0)
            {
                // Iteration over elements
                while (block.value<=mEnd)
                {
                    if (!block.yield(block.value))
                    {
                        cerr << "yield failed for Sequence#each on case " << block.index << ", make sure yield returns true" << endl;
                        return false;
                    }
                    block.index++;
                    block.value+=mIncrement;
                }
            } else
            {
                // Iteration over elements
                while (block.value>=mEnd)
                {
                    if (!block.yield(block.value))
                    {
                        cerr << "yield failed for Sequence#each on case " << block.index << ", make sure yield returns true" << endl;
                        return false;
                    }
                    block.index++;
                    block.value+=mIncrement;
                }
            }

            // Post
            if (!block.postYield())
            {
                cerr << "postYield failed for Sequence#each, make sure postYield returns true" << endl;
                return false;
            }
            return true;
        }

    template<typename Block>
    vector<typename Block::ResultT> *collect(Block &block)
        {
            T value;
            block.pSequence = this;
            block.begin=mBegin;
            block.end=mEnd;
            block.increment=mIncrement;
            block.index=0;
            block.value=mBegin;
            vector<typename Block::ResultT> *pRes = new vector<typename Block::ResultT>;

            // Pre
            if (!block.preYield())
            {
                cerr << "preYield failed for Sequence#collect, make sure preYield returns true" << endl;
                delete pRes;
                return NULL;
            }

            if (mIncrement>0)
            {
                // Iteration over elements
                while (block.value<=mEnd)
                {
                    pRes->push_back(block.value);
                    if (!block.yield((*pRes)[block.index],block.value))
                    {
                        cerr << "yield failed for Sequence#collect on case " << block.index << ", make sure yield returns true" << endl;
                        return false;
                    }
                    block.index++;
                    block.value+=mIncrement;
                }
            } else
            {
                // Iteration over elements
                while (block.value>=mEnd)
                {
                    pRes->push_back(block.value);
                    if (!block.yield((*pRes)[block.index],block.value))
                    {
                        cerr << "yield failed for Sequence#collect on case " << block.index << ", make sure yield returns true" << endl;
                        delete pRes;
                        return NULL;
                    }
                    block.index++;
                    block.value+=mIncrement;
                }
            }

            // Post
            if (!block.postYield())
            {
                cerr << "postYield failed for Sequence#collect, make sure postYield returns true" << endl;
                delete pRes;
                return NULL;
            }
            return pRes;
        }

private:
    T mBegin;
    T mEnd;
    T mIncrement;
};

template<typename T>
Sequence<T>::Sequence(T begin, T end, T increment)
{
    mBegin=begin;
    mEnd=end+increment;
    mIncrement=increment;
}

template<>
Sequence<double>::Sequence(double begin, double end, double increment)
{
    mBegin=begin;
    mEnd=end+0.5*increment;
    mIncrement=increment;
}


#endif
