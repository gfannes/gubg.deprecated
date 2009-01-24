#ifndef file_h
#define file_h

#include <string>
#include <fstream>

using namespace std;

namespace File
{
    bool readAll(char *&data,string fileName)
    {
        // Open the file
        ifstream fi(fileName.c_str(),ios::binary);
        // Get the file size
        int size;
        if (fi.is_open())
        {
            long begin,end;
            begin = fi.tellg();
            fi.seekg(0, ios::end);
            end = fi.tellg();
            size = end-begin;
        } else
        {
            cerr << "Could not open file " << fileName << endl;
            return false;
        }
        // Allocate data
        data = new char[size];
        // Read the file into data
        if (data)
        {
            fi.seekg(0, ios::beg);
            fi.read(data,size);
        }
        fi.close();
        return data != NULL;
    }

};

#endif
