#ifndef thread_h
#define thread_h

#include "pthread.h"

class Thread
{
public:
    Thread():
        mRunning(false){};
    int start();
    void finish();
    bool isRunning(){return mRunning;};
protected:
    static void *staticDummy(void *);
    virtual bool execute() = 0;
    void setRunning(bool b){mRunning = b;};
private:
    bool mRunning;
    pthread_t mThread;
};

template <typename T>
class Mutex
{
public:
    Mutex(T *pResource):
        mResource(pResource){pthread_mutex_init(&mMutex, NULL);};
//    virtual bool execute() = 0;
    template <typename T2>
    bool access(T2 &t2)
        {
            bool ok;
            pthread_mutex_lock(&mMutex);
            ok = t2.execute4Mutex(mResource);
            pthread_mutex_unlock(&mMutex);
            return ok;
        }
    void lock()
        {
            pthread_mutex_lock(&mMutex);
        }
    void unlock()
        {
            pthread_mutex_unlock(&mMutex);
        }
private:
    pthread_mutex_t mMutex;
    T *mResource;
};

#endif
