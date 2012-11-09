#ifndef HEADER_gubg_threading_thread_old_hpp_ALREADY_INCLUDED
#define HEADER_gubg_threading_thread_old_hpp_ALREADY_INCLUDED

#include "pthread.h"

// You can create a thread by inheriting from Thread and providing the method:
// virtual bool Thread::execute();
class Thread
{
    public:
        Thread():
            _running(false){};
        int start();
        void finish();
        bool isRunning(){return _running;};
    protected:
        static void *staticDummy(void *);
        virtual bool execute() = 0;
        void setRunning(bool b){_running = b;};
    private:
        bool _running;
        pthread_t _thread;
};

// Locks a resource of type ResourceType away behind a mutex.
// You can access the resource by providing a class ResourceRequester that has at least:
// class ResourceRequester
// {
// public:
//   bool execute4Mutex(ResourceType *resource){}
// };
template <typename ResourceType>
class Mutex
{
    public:
        Mutex(ResourceType *resource):
            _resource(resource){pthread_mutex_init(&_mutex, NULL);};

        template <typename ResourceRequester>
            bool access(ResourceRequester &resourceRequester)
            {
                bool ok;
                pthread_mutex_lock(&_mutex);
                ok = resourceRequester.execute4Mutex(_resource);
                pthread_mutex_unlock(&_mutex);
                return ok;
            }

        void lock()
        {
            pthread_mutex_lock(&_mutex);
        }
        void unlock()
        {
            pthread_mutex_unlock(&_mutex);
        }

    private:
        pthread_mutex_t _mutex;
        ResourceType *_resource;
};

#endif
