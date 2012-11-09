#ifndef HEADER_gubg_sequence_sequence_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sequence_sequence_hpp_ALREADY_INCLUDED

#include <vector>
#include <iostream>

#define nullptr ((void*)0)

namespace gubg
{
    template<typename T>
        class Sequence
        {
            public:
                struct EachBlock
                {
                    bool preYield(){return true;}
                    bool yield(T v){return true;}
                    bool postYield(){return true;}

                    int index;
                    T value;
                    T begin;
                    T end;
                    T increment;
                    Sequence<T> *sequence_;
                };
                template<typename TResult>
                    struct CollectBlock
                    {
                        bool preYield(){return true;};
                        bool yield(TResult &res,T v){return true;};
                        bool postYield(){return true;};

                        typedef TResult ResultT;
                        int index;
                        T value;
                        T begin;
                        T end;
                        T increment;
                        Sequence<T> *sequence_;
                    };
                Sequence(T end)
                {
                    begin_ = 0;
                    end_ = end - 1;
                    increment_ = 1;
                }
                Sequence(T begin, T end, T increment = 1);

                template<typename Block>
                    bool each(Block &block)
                    {
                        T value;
                        block.sequence_ = this;
                        block.begin = begin_;
                        block.end = end_;
                        block.increment = increment_;
                        block.index = 0;
                        block.value = begin_;

                        // Pre
                        if (!block.preYield())
                        {
                            std::cerr << "preYield failed for Sequence#each, make sure preYield returns true" << std::endl;
                            return false;
                        }

                        if (increment_ > 0)
                        {
                            //Iteration over elements
                            while (block.value <= end_)
                            {
                                if (!block.yield(block.value))
                                {
                                    std::cerr << "yield failed for Sequence#each on case " << block.index << ", make sure yield returns true" << std::endl;
                                    return false;
                                }
                                ++block.index;
                                block.value += increment_;
                            }
                        }
                        else
                        {
                            //Iteration over elements
                            while (block.value >= end_)
                            {
                                if (!block.yield(block.value))
                                {
                                    std::cerr << "yield failed for Sequence#each on case " << block.index << ", make sure yield returns true" << std::endl;
                                    return false;
                                }
                                ++block.index;
                                block.value += increment_;
                            }
                        }

                        //Post
                        if (!block.postYield())
                        {
                            std::cerr << "postYield failed for Sequence#each, make sure postYield returns true" << std::endl;
                            return false;
                        }
                        return true;
                    }

                template<typename Block>
                    std::auto_ptr<std::vector<typename Block::ResultT>> collect(Block &block)
                    {
                        T value;
                        block.sequence_ = this;
                        block.begin = begin_;
                        block.end = end_;
                        block.increment = increment_;
                        block.index = 0;
                        block.value = begin_;
                        std::auto_ptr<std::vector<typename Block::ResultT>> res;

                        // Pre
                        if (!block.preYield())
                        {
                            std::cerr << "preYield failed for Sequence#collect, make sure preYield returns true" << std::endl;
                            return res;
                        }

                        res.reset(new std::vector<typename Block::ResultT>);
                        if (increment_>0)
                        {
                            // Iteration over elements
                            while (block.value <= end_)
                            {
                                res->push_back(block.value);
                                if (!block.yield((*res)[block.index],block.value))
                                {
                                    std::cerr << "yield failed for Sequence#collect on case " << block.index << ", make sure yield returns true" << std::endl;
                                    res.reset();
                                    return res;
                                }
                                ++block.index;
                                block.value += increment_;
                            }
                        } else
                        {
                            // Iteration over elements
                            while (block.value >= end_)
                            {
                                res->push_back(block.value);
                                if (!block.yield((*res)[block.index],block.value))
                                {
                                    std::cerr << "yield failed for Sequence#collect on case " << block.index << ", make sure yield returns true" << std::endl;
                                    res.reset();
                                    return res;
                                }
                                ++block.index;
                                block.value += increment_;
                            }
                        }

                        // Post
                        if (!block.postYield())
                        {
                            std::cerr << "postYield failed for Sequence#collect, make sure postYield returns true" << std::endl;
                            res.reset();
                            return res;
                        }
                        return res;
                    }

            private:
                T begin_;
                T end_;
                T increment_;
        };

    template<typename T>
        Sequence<T>::Sequence(T begin, T end, T increment)
        {
            begin_ = begin;
            end_ = end + increment;
            increment_ = increment;
        }

    template<>
        Sequence<float>::Sequence(float begin, float end, float increment)
        {
            begin_ = begin;
            end_ = end + 0.5*increment;
            increment_ = increment;
        }

    template<>
        Sequence<double>::Sequence(double begin, double end, double increment)
        {
            begin_ = begin;
            end_ = end + 0.5*increment;
            increment_ = increment;
        }
}

#endif
