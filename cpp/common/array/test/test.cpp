#include <iostream>
#include <vector>

#include "array.hpp"

using namespace std;

class Printer: public Array<int>::EachBlock
{
public:
    bool preYield(int &val)
        {
            cout << "\tPrinter will start with: " << val << " at index " << index << endl;
            return true;
        }
    bool yield(int &val)
        {
            cout << "\tPrinter[" << index << "] == " << val << endl;
            return true;
        }
};

class Printer2: public Array<int>::EachBlock
{
public:
    bool yield(int &val)
        {
            cout << "Printer[" << index << "] == " << val << endl;
            Printer printer;
            pArray->each(printer);
            return true;
        }
};

class Collector: public Array<int>::CollectBlock<int>
{
public:
    bool yield(int &res,int &val)
        {
            res = val*val;
            return true;
        }
};

class Each2: public Array<int,int>::EachBlock
{
public:
    bool yield(int &v1, int &v2)
        {
            cout << "Each for two: " << v1 << " " << v2 << endl;
            return true;
        }
};

class Collect2: public Array<int,int>::CollectBlock<int>
{
public:
    bool yield(int &res, int &v1, int &v2)
        {
            res=v1+v2;
            return true;
        }
};

int main()
{
    vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);
    Array<int> array(v1);
    Printer printer;
    Printer2 printer2;
    array.each(printer2);
    vector<int> *pVec;
    Collector collector;
    pVec=array.collect(collector);
    array.set(*pVec);
    array.each(printer);
    Array<int,int> array2(v1,*pVec);
    Each2 each2;
    array2.each(each2);
    Collect2 collect2;
    pVec=array2.collect(collect2);
    array.set(*pVec);
    array.each(printer);
    return 0;
}
