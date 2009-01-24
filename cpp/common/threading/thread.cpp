#include "thread.hpp"

int Thread::start()
{
    setRunning(true);
    return pthread_create(&mThread, NULL, staticDummy, (void *)this);
}

void *Thread::staticDummy(void *data)
{
    Thread *thread = ((Thread*)data);
    thread->execute();
    thread->setRunning(false);
}

void Thread::finish()
{
    pthread_join(mThread, NULL);
}
