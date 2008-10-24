#ifndef comPoint_h
#define comPoint_h

// The description of a ComStation, used to specify where messages should be sent to

#include <string>

using namespace std;

class ComPoint
{
public:
    ComPoint(string hostname, int port):
        mHostname(hostname),
        mPort(port){};
    string toStr();
    int port(){return mPort;};
    string hostname(){return mHostname;};
private:
    string mHostname;
    int mPort;
};
#endif
