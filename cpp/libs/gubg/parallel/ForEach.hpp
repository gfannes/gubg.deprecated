#ifndef HEADER_gubg_parallel_ForEach_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parallel_ForEach_hpp_ALREADY_INCLUDED

#include <thread>
#include <mutex>
#include <list>

#define GUBG_MODULE "ForEach"
#include "gubg/log/begin.hpp"
namespace gubg { namespace parallel { 

    namespace detail { 
        template <typename Iterator, typename Ftor>
        class Thread
        {
            public:
                Thread(Iterator &b, Iterator e, std::mutex &m, Ftor &ftor): b_(b), e_(e), m_(m), ftor_(ftor), thread_(std::ref(*this))
            {
                S();L("ctor " << this);
            }
                Thread(Thread &&rhs): b_(rhs.b_), e_(rhs.e_), m_(rhs.m_), ftor_(rhs.ftor_), thread_(std::move(rhs.thread_))
            {
                S();L("mtor " << this << " from " << &rhs);
            }
                ~Thread()
                {
                    S();L("dtor " << this);
                    if (thread_.joinable())
                        thread_.join();
                }
                void operator()()
                {
                    while (true)
                    {
                        Iterator it;
                        {
                            std::lock_guard<std::mutex> lock(m_);
                            if (b_ == e_)
                                return;
                            it = b_++;
                        }
                        ftor_(*it);
                    }
                }
            private:
                Thread(const Thread &);
                Thread &operator=(const Thread &);

                Iterator &b_, e_;
                std::mutex &m_;
                Ftor &ftor_;
                std::thread thread_;
        };
    } 

    template <typename Iterator, typename Ftor>
        void for_each(Iterator b, Iterator e, Ftor ftor, size_t nrThreads = 0)
        {
			if (nrThreads == 0)
				nrThreads = std::thread::hardware_concurrency();
			if (nrThreads == 0)
				nrThreads = 1;

            std::mutex m;
            typedef detail::Thread<Iterator, Ftor> Thread;
            typedef std::list<Thread> Threads;
            Threads threads;

            for (size_t i = 0; i < nrThreads; ++i)
                threads.push_back(Thread(b, e, m, ftor));
        }

} } 
#include "gubg/log/end.hpp"

#endif
