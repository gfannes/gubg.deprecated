#ifndef comStation_h
#define comStation_h

#include <list>

#include "socket.hpp"
#include "comPoint.hpp"
#include "thread.hpp"
#include "serialize.hpp"

using namespace std;

class Message;
typedef list<Message*> MessageListT;

// A communication station that includes:
//  * a server that keeps listening
//  * a message queue that contains unprocessed messages
//  * a callback mechanism to process certain messages, according to their type
//  * the central place to send messages to other ComPoints
//  * much more

class AcceptThread: public Thread
{
public:
    AcceptThread(int listenSocket, bool *continue2Listen, Mutex<MessageListT> *stagingM):
        mListenSocket(listenSocket),
        mContinue2Listen(continue2Listen),
        mStagingM(stagingM){};
protected:
    virtual bool execute();
private:
    int mListenSocket;
    bool *mContinue2Listen;
    Mutex<MessageListT> *mStagingM;
};

class ComStation
{
public:
    ComStation(ComPoint compoint,int nrPending=20):
        mHere(compoint),
        mNrPending(nrPending),
        mAcceptThread(NULL),
        mMessagesM(&mMessages),
        mStagingM(&mStaging){};
    ~ComStation();
    bool start();
    bool finish();
    template <typename T>
    static bool send(ComPoint compoint, T &t);
    static bool write(ComPoint compoint, string &buffer);
    static bool read(int fd,string &buffer);
    template <typename T>
    bool checkMessages(T &t);

private:
    static bool write(int fd,string &buffer);
    static bool writeBuffer(int fd,char const *buffer, int bytes2Send);
    static bool readBuffer(int fd,char *buffer, int bytes2Receive);
    bool moveStaging2Messages();
    AcceptThread *mAcceptThread;
    bool mContinue2Listen;

    MessageListT mMessages;
    Mutex<MessageListT> mMessagesM;
    MessageListT mStaging;
    Mutex<MessageListT> mStagingM;
    ComPoint mHere;
    int mListenSocket;
    struct sockaddr_in mAddress;    // my address information
    int mNrPending;
};

template <typename T>
bool ComStation::send(ComPoint compoint, T &t)
{
    bool ok=true;
    ostringstream oss;
    string str;
    ok &= Serialize::serializeWithType(t, oss);
    str = oss.str();
    ok &= write(compoint, str);
    return ok;
}

template <typename T>
bool ComStation::checkMessages(T &t)
{
    bool ok;
    ok = moveStaging2Messages();
    cout << "moving is done " << mMessages.size() << mStaging.size() << endl;
    ok &= mMessagesM.access(t);
    return ok;
}
#endif
