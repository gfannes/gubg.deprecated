#include "threading/InstanceCounter.hpp"
#include "threading/ProducerConsumer.hpp"
#include "testing/Testing.hpp"
#define GUBG_LOG
#include "logging/Log.hpp"
#include <iostream>
#include <string>
using namespace std;

struct Thread: gubg::InstanceCounter<Thread>
{
    Thread():
        thread_(boost::ref(*this)){}
    void operator()()
    {
        {
            boost::mutex::scoped_lock lock(countMutex);
            ++count;
        }
        delete this;
    }
    boost::thread thread_;
    static int count;
    static boost::mutex countMutex;
};
int Thread::count = 0;
boost::mutex Thread::countMutex;

struct Message
{
    Message(bool c):
        continue_(c){}
    int continue_;
};
typedef gubg::threading::Queue<Message> QueueT;
struct Producer: gubg::InstanceCounter<Producer>
{
    Producer(int nrMessages, QueueT &queue):
        nrMessages_(nrMessages),
        queue_(queue),
        thread_(boost::ref(*this)){}
    void operator()()
    {
        LOG_SM(Producer, "Starting up");
        for (int i = 0; i < nrMessages_; ++i)
            queue_.push(unique_ptr<Message>(new Message(true)));
        LOG_M("Time the go");
        delete this;
    }
    int nrMessages_;
    QueueT &queue_;
    boost::thread thread_;
};
struct Consumer: gubg::InstanceCounter<Consumer>
{
    Consumer(QueueT &queue):
        queue_(queue),
        thread_(boost::ref(*this)){}
    void operator()()
    {
        LOG_SM(Consumer, "Starting up");
        try
        {
            while (true)
            {
                auto message = queue_.pop();
                if (!message->continue_)
                    break;
                ++messageCount;
            }
        }
        catch (QueueT::Closed &)
        {
            LOG_M("The queue is closed");
        }
        LOG_M("Time the go");
        delete this;
    }
    QueueT &queue_;
    boost::thread thread_;
    static int messageCount;
};
int Consumer::messageCount = 0;

int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(InstanceCounter);
        //Create and start the threads
        const int NrThreads = 1000;
        for (int i = 0; i < NrThreads; ++i)
            new Thread;
        //Wait until the threads are all done
        while (Thread::nrInstances() > 0){}
        TEST_EQ(NrThreads, Thread::count);
    }
    {
        TEST_TAG(ProducerConsumer);
        QueueT queue;
        //Start the producers
        const int NrMessagesPerProducer = 10000;
        const int NrProducers = 100;
        for (int i = 0; i < NrProducers; ++i)
            new Producer(NrMessagesPerProducer, queue);
        //Start the consumer
        new Consumer(queue);
        //Wait until all producers are gone
        while (Producer::nrInstances() > 0){}
        //Indicate to the consumer to quit
        queue.push(unique_ptr<Message>(new Message(false)));
        //Wait until the consumer is gone
        while (Consumer::nrInstances() > 0){}
        //Indicate that the queue is closed, this should not be a problem, the consumer is already gone by now
        queue.close();
        TEST_EQ(NrMessagesPerProducer*NrProducers, Consumer::messageCount);
    }
    return 0;
}
