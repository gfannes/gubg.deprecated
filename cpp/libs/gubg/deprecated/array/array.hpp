#ifndef HEADER_gubg_deprecated_array_array_hpp_ALREADY_INCLUDED
#define HEADER_gubg_deprecated_array_array_hpp_ALREADY_INCLUDED

#include <vector>

using namespace std;

class Nothing
{
};

template<typename T, typename T2=Nothing>
class Array
{
public:
    class EachBlock
    {
    public:
        bool preYield(T &t, T2 &t2){return true;};
        bool yield(T &t, T2 &t2){return true;};
        bool postYield(){return true;};

        int index;
        Array<T,T2> *pArray;
    };
    template<typename TResult>
    class CollectBlock
    {
    public:
        bool preYield(T &t, T2 &t2){return true;};
        bool yield(TResult &res,T &t, T2 &t2){return true;};
        bool postYield(){return true;};

        typedef TResult ResultT;
        int index;
        Array<T,T2> *pArray;
    };
    Array(){}
    Array(vector<T> &t,vector<T2> &t2){mpVector = &t;mpVector2 = &t2;}
    void set(vector<T> &t,vector<T2> &t2){mpVector = &t;mpVector2 = &t2;}
    template<typename Block>
    bool each(Block &block)
        {
            int i,nr=mpVector->size();
            block.pArray = this;

            // Pre
            block.index = 0;
            if (!block.preYield((*mpVector)[0],(*mpVector2)[0]))
            {
                cerr << "preYield failed for Array#each, make sure preYield returns true" << endl;
                return false;
            }

            // Iteration over elements
            for (i=0;i<nr;i++)
            {
                block.index = i;
                if (!block.yield((*mpVector)[i],(*mpVector2)[i]))
                {
                    cerr << "yield failed for Array#each on case " << i << ", make sure yield returns true" << endl;
                    return false;
                }
            }

            // Post
            block.index = nr;
            if (!block.postYield())
            {
                cerr << "postYield failed for Array#each, make sure postYield returns true" << endl;
                return false;
            }
            return true;
        }
    template<typename Block>
    vector<typename Block::ResultT> *collect(Block &block)
        {
            int i,nr=mpVector->size();
            block.pArray = this;
            vector<typename Block::ResultT> *pRes = new vector<typename Block::ResultT>;
            pRes->resize(nr);

            // Pre
            block.index = 0;
            if (!block.preYield((*mpVector)[0],(*mpVector2)[0]))
            {
                cerr << "preYield failed for Array#collect, make sure preYield returns true" << endl;
                delete pRes;
                return NULL;
            }


            // Iteration over elements
            for (i=0;i<nr;i++)
            {
                block.index = i;
                if (!block.yield((*pRes)[i],(*mpVector)[i],(*mpVector2)[i]))
                {
                    cerr << "yield failed for Array#collect on case " << i << ", make sure yield returns true" << endl;
                delete pRes;
                    return NULL;
                }
            }

            // Post
            block.index = nr;
            if (!block.postYield())
            {
                cerr << "postYield failed for Array#collect, make sure postYield returns true" << endl;
                delete pRes;
                return NULL;
            }

            return pRes;
        }
private:
    vector<T> *mpVector;
    vector<T2> *mpVector2;
};

template<typename T>
class Array<T,Nothing>
{
public:
    class EachBlock
    {
    public:
        bool preYield(T &t){return true;};
        bool yield(T &t){return true;};
        bool postYield(){return true;};

        int index;
        Array<T> *pArray;
    };
    template<typename TResult>
    class CollectBlock
    {
    public:
        bool preYield(T &t){return true;};
        bool yield(TResult &res,T &t){return true;};
        bool postYield(){return true;};

        typedef TResult ResultT;
        int index;
        Array<T> *pArray;
    };
    Array(){}
    Array(vector<T> &t){mpVector = &t;}
    void set(vector<T> &t){mpVector = &t;}
    template<typename Block>
    bool each(Block &block)
        {
            int i,nr=mpVector->size();
            block.pArray = this;

            // Pre
            block.index = 0;
            if (!block.preYield((*mpVector)[0]))
            {
                cerr << "preYield failed for Array#each, make sure preYield returns true" << endl;
                return false;
            }

            // Iteration over elements
            for (i=0;i<nr;i++)
            {
                block.index = i;
                if (!block.yield((*mpVector)[i]))
                {
                    cerr << "yield failed for Array#each on case " << i << ", make sure yield returns true" << endl;
                    return false;
                }
            }

            // Post
            block.index = nr;
            if (!block.postYield())
            {
                cerr << "postYield failed for Array#each, make sure postYield returns true" << endl;
                return false;
            }
            return true;
        }
    template<typename Block>
    vector<typename Block::ResultT> *collect(Block &block)
        {
            int i,nr=mpVector->size();
            block.pArray = this;
            vector<typename Block::ResultT> *pRes = new vector<typename Block::ResultT>;
            pRes->resize(nr);

            // Pre
            block.index = 0;
            if (!block.preYield((*mpVector)[0]))
            {
                cerr << "preYield failed for Array#collect, make sure preYield returns true" << endl;
                delete pRes;
                return NULL;
            }

            // Iteration over elements
            for (i=0;i<nr;i++)
            {
                block.index = i;
                if (!block.yield((*pRes)[i],(*mpVector)[i]))
                {
                    cerr << "yield failed for Array#collect on case " << i << ", make sure yield returns true" << endl;
                    delete pRes;
                    return NULL;
                }
            }

            // Post
            block.index = nr;
            if (!block.postYield())
            {
                cerr << "postYield failed for Array#collect, make sure postYield returns true" << endl;
                delete pRes;
                return NULL;
            }

            return pRes;
        }
private:
    vector<T> *mpVector;
};

#endif
