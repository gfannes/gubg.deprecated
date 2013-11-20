#include "gubg/threading/InstanceCounter.hpp"
#include "gubg/threading/Queue.hpp"
#include "gubg/Testing.hpp"
#include <iostream>
#include <string>
using namespace std;

struct Thread: gubg::InstanceCounter<Thread>
{
    Thread():
        thread_(std::ref(*this)){}
    void operator()()
    {
        {
            std::lock_guard<std::mutex> lock(countMutex);
            ++count;
        }
        delete this;
    }
    std::thread thread_;
    static int count;
    static std::mutex countMutex;
};
int Thread::count = 0;
std::mutex Thread::countMutex;

struct Message
{
    Message(bool c):
        continue_(c){}
    int continue_;
};
#define GUBG_MODULE "Producer"
#include "gubg/log/begin.hpp"
typedef gubg::threading::Queue<Message> QueueT;
struct Producer: gubg::InstanceCounter<Producer>
{
    Producer(int nrMessages, QueueT &queue):
        nrMessages_(nrMessages),
        queue_(queue),
        thread_(std::ref(*this)){}
    void operator()()
    {
        S();L("Starting up");
        for (int i = 0; i < nrMessages_; ++i)
            queue_.push(unique_ptr<Message>(new Message(true)));
        L("Time the go");
        delete this;
    }
    int nrMessages_;
    QueueT &queue_;
    std::thread thread_;
};
#include "gubg/log/end.hpp"

#define GUBG_MODULE "Consumer"
#include "gubg/log/begin.hpp"
struct Consumer: gubg::InstanceCounter<Consumer>
{
    Consumer(QueueT &queue):
        queue_(queue),
        thread_(std::ref(*this)){}
    void operator()()
    {
        S();L("Starting up");
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
            L("The queue is closed");
        }
        L("Time the go");
        delete this;
    }
    QueueT &queue_;
    std::thread thread_;
    static int messageCount;
};
#include "gubg/log/end.hpp"
int Consumer::messageCount = 0;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
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
#include "gubg/log/end.hpp"
