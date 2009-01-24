#ifndef image_h
#define image_h

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "file.hpp"
#include "vector.hpp"
#include "cube.hpp"

using namespace std;

class Image
{
public:
    Image(string fileName):
        mValid(false)
        {loadImage(fileName);}
    bool loadImage(string fileName)
        {
            mFileName = fileName;
            // Read the image in binary buffer
            char *data = NULL;
            if (!File::readAll(data,mFileName))
            {
                cerr << "Could not load image " << mFileName << endl;
                return false;
            }

            bool ok = true;
            // Extract dimension and bpp information
            mWidth = *reinterpret_cast<unsigned int *>(data+18);
            mHeight = *reinterpret_cast<unsigned int *>(data+22);
            mBPP = *reinterpret_cast<unsigned short int *>(data+28);
            // Read the image data into mData
            int i,j;
            mData.resize(mHeight);
            for (i=0;i<mHeight;i++) mData[i].resize(mWidth);
            int nrBytesPerLine=(((mWidth*mBPP+7)/8+3)/4)*4;
            int dataoffset=*reinterpret_cast<unsigned int *>(data+10);
            char *p;
            switch (mBPP)
            {
            case 8:
                for (i=0;i<mHeight;i++)
                {
                    p = data+dataoffset+nrBytesPerLine*i;
                    for (j=0;j<mWidth;j++)
                        mData[i][j] = *reinterpret_cast<unsigned char *>(p+j);
                }
                break;
            default:
                cerr << "Unknown bbp " << mBPP << endl;
                ok = false;
                break;
            };
            // Release the raw image data
            if (data)
                delete[] data;
            mValid = ok;
            return ok;
        }
    bool valid() const{return mValid;}
    string toString() const
        {
            ostringstream ostr;
            if (valid())
            {
                ostr << "Image was read from " << mFileName << " and has dimensions (" << mWidth << "," << mHeight << ") and BPP = " << mBPP;
                char chars[]={' ','.',',',';','-','=','x','*','X'};
                int mi,ma,i,j;
                Cube::findMinMax(mi,ma,mData);
                double a,b;
                a = double(sizeof(chars)-1)/((mi-ma)==0 ? 2 : (mi-ma));
                b = -a*ma;
                ostr << endl;
                for (j=0;j<mWidth+2;j++)
                    ostr << '-';
                for (i=0;i<mHeight;i++)
                {
                    ostr << endl;
                    ostr << '|';
                    for (j=0;j<mWidth;j++)
                        ostr << chars[int(a*mData[i][j]+b)];
                    ostr << '|';
                }
                ostr << endl;
                for (j=0;j<mWidth+2;j++)
                    ostr << '-';
            } else ostr << "This image object is not valid";
            return ostr.str();
        }
    vector<vector<int> > &data(){return mData;};
private:
    bool mValid;
    string mFileName;
    unsigned int mWidth;
    unsigned int mHeight;
    unsigned short int mBPP;
    vector<vector<int> > mData;
};

basic_ostream<char> &operator<<(basic_ostream<char> &os, const Image &img)
{
    os << img.toString();
    return os;
}

#endif
