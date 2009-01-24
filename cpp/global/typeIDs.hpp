#ifndef typeids_h
#define typeids_h

// This file contains the serialization type ids for this project

#include <iostream>
using namespace std;
namespace Serialize
{
    enum TypeIDH
    {
        eTypeIDint,
        eTypeIDlong,
        eTypeIDdouble,
        eTypeIDstring,
        eTypeIDvector,
        eTypeIDVectorInt,
        eTypeIDVectorLong,
        eTypeIDVectorDouble,
        eTypeIDVectorVectorDouble,
        eTypeIDVectorVectorVectorDouble,
        eTypeIDMessage,
        eTypeIDNeuronTanh,
        eTypeIDNeuronSigmoid,
        eTypeIDNeuronLinear,
        eTypeIDMatrixInt,
        eTypeIDMatrixLong,
        eTypeIDMatrixDouble,
        eTypeIDVectorMatrixInt,
        eTypeIDVectorMatrixLong,
        eTypeIDVectorMatrixDouble,
        eTypeIDCRPoint,
        eTypeIDVectorCRPoint,
        eTypeIDConvexRegion,
        eTypeIDVectorConvexRegion,
    };

    // int
    struct BasicMethodsInt
    {
        static int* allocate(){return (new int);};
        static bool serialize(int &i,ostream &output){output.write(reinterpret_cast<const char*>(&i),4);return true;};
        static bool unserialize(int &i,istream &input){input.read(reinterpret_cast<char*>(&i),4);return true;};
    };
    SetTypeAndFunctions(int,eTypeIDint,BasicMethodsInt::allocate,BasicMethodsInt::serialize,BasicMethodsInt::unserialize);

    // long
    struct BasicMethodsLong
    {
        static long* allocate(){return (new long);};
        static bool serialize(long &i,ostream &output){output.write(reinterpret_cast<const char*>(&i),4);return true;};
        static bool unserialize(long &i,istream &input){input.read(reinterpret_cast<char*>(&i),4);return true;};
    };
    SetTypeAndFunctions(long,eTypeIDlong,BasicMethodsLong::allocate,BasicMethodsLong::serialize,BasicMethodsLong::unserialize);

    // double
    struct BasicMethodsDouble
    {
        static double* allocate(){return (new double);};
        static bool serialize(double &d,ostream &output)
            {
                output.write(reinterpret_cast<const char*>(&d),8);
                return true;
            }
        static bool unserialize(double &d,istream &input){input.read(reinterpret_cast<char*>(&d),8);return true;};
    };
    SetTypeAndFunctions(double,eTypeIDdouble,BasicMethodsDouble::allocate,BasicMethodsDouble::serialize,BasicMethodsDouble::unserialize);

    // string
    struct BasicMethodsString
    {
        static string* allocate(){return (new string);};
        static bool serialize(string &str,ostream &output)
            {
                static int len;
                len=str.length();
                Serialize::serialize(len,output);
                output.write(str.data(),len);
                return true;
            };
        static bool unserialize(string &str,istream &input)
            {
                static int len;
                if (!Serialize::unserialize(len,input))
                {
                    cerr << "Could not read the length of the string" << endl;
                    return false;
                }
                str.resize(len);
                input.read(const_cast<char*>(str.data()),len);
                return true;
            };
    };
    SetTypeAndFunctions(string,eTypeIDstring,BasicMethodsString::allocate,BasicMethodsString::serialize,BasicMethodsString::unserialize);

    // vector<T>
    template <typename T>
    struct BasicMethodsVector
    {
        static vector<T>* allocate(){return (new vector<T>);};
        static bool serialize(vector<T> &vec,ostream &output)
            {
                int len,i;
                len=vec.size();
                Serialize::serialize(len,output);
                for (i=0;i<len;i++)
                {
                    if (!Serialize::serialize(vec[i],output))
                        break;
                }
                if (i==len)
                    return true;
                else
                    return false;
            };
        static bool unserialize(vector<T> &vec,istream &input)
            {
                int len,i;
                if (!Serialize::unserialize(len,input))
                {
                    cerr << "Could not read the length of the vector<T>" << endl;
                    return false;
                }
                vec.resize(len);
                for (i=0;i<len;i++)
                    if (!Serialize::unserialize(vec[i],input))
                        break;
                if (i==len)
                    return true;
                else
                    return false;
            };
    };
    SetTypeAndFunctions(vector<int>,eTypeIDVectorInt,BasicMethodsVector<int>::allocate,BasicMethodsVector<int>::serialize,BasicMethodsVector<int>::unserialize);
    SetTypeAndFunctions(vector<long>,eTypeIDVectorLong,BasicMethodsVector<long>::allocate,BasicMethodsVector<long>::serialize,BasicMethodsVector<long>::unserialize);
    SetTypeAndFunctions(vector<double>,eTypeIDVectorDouble,BasicMethodsVector<double>::allocate,BasicMethodsVector<double>::serialize,BasicMethodsVector<double>::unserialize);
    SetTypeAndFunctions(vector<vector<double> >,eTypeIDVectorVectorDouble,BasicMethodsVector<vector<double> >::allocate,BasicMethodsVector<vector<double> >::serialize,BasicMethodsVector<vector<double> >::unserialize);
    SetTypeAndFunctions(vector<vector<vector<double> > >,eTypeIDVectorVectorVectorDouble,BasicMethodsVector<vector<vector<double> > >::allocate,BasicMethodsVector<vector<vector<double> > >::serialize,BasicMethodsVector<vector<vector<double> > >::unserialize);
};

#endif
