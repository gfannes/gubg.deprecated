#ifndef HEADER_gubg_cnt_ptr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cnt_ptr_hpp_ALREADY_INCLUDED

//!@#adoc
//cnt_ptr is meant as a non-thread-safe alternative to std::shared_ptr, but in its current implementation,
//it does not yet save you memory, nor is it much faster. And the weak version is also lacking, although
//there is already a counter for this

#include <cassert>

namespace gubg { 

    template <typename T>
        class cnt_ptr
        {
            public:
                cnt_ptr();
                explicit cnt_ptr(T *);
                cnt_ptr(const cnt_ptr<T> &);
                cnt_ptr &operator=(const cnt_ptr<T> &);

                ~cnt_ptr();

                void reset();
                void reset(T *);

                explicit operator bool() const;

                T *operator->() const;
                T &operator*() const;

            private:
                bool invariants_() const;
                //Does not respect invariants_() for speed
                void decrement_();

                T *ptr_ = nullptr;

                struct Ref
                {
                    typedef unsigned int Count;
                    //For speed, we assume you want to set the ref count to 1 initially
                    Count share = 1;
                    Count weak  = 0;
                };
                Ref *ref_ = nullptr;
        };

    //Implementation
    template <typename T>
        cnt_ptr<T>::cnt_ptr() { assert(invariants_()); }
    template <typename T>
        cnt_ptr<T>::cnt_ptr(T *p): ptr_(p), ref_(new Ref) { assert(invariants_()); }
    template <typename T>
        cnt_ptr<T>::cnt_ptr(const cnt_ptr<T> &rhs): ptr_(rhs.ptr_), ref_(rhs.ref_)
        {
            assert(invariants_());
            if (ptr_)
                ++ref_->share;
            assert(invariants_());
        }
    template <typename T>
        cnt_ptr<T> & cnt_ptr<T>::operator=(const cnt_ptr<T> &rhs)
        {
            assert(invariants_());
            assert(rhs.invariants_());
            if (&rhs != this)
            {
                decrement_();
                ptr_ = rhs.ptr_;
                ref_ = rhs.ref_;
                if (ptr_)
                    ++ref_->share;
            }
            assert(invariants_());
            return *this;
        }
    template <typename T>
        cnt_ptr<T>::~cnt_ptr()
        {
            assert(invariants_());
            decrement_();
        }
    template <typename T>
        void cnt_ptr<T>::reset()
        {
            assert(invariants_());
            decrement_();
            ptr_ = nullptr;
            ref_ = nullptr;
            assert(invariants_());
        }
    template <typename T>
        void cnt_ptr<T>::reset(T *p)
        {
            assert(invariants_());
            decrement_();
            ptr_ = p;
            if (ptr_)
                ref_ = new Ref;
            else
                ref_ = nullptr;
            assert(invariants_());
        }
    template <typename T>
        void cnt_ptr<T>::decrement_()
        {
            if (ref_)
            {
                if (ref_->share == 1)
                {
                    delete ptr_;
                    if (ref_->weak == 0)
                        delete ref_;
                    else
                        ref_->share = 0;
                }
                else
                    --ref_->share;
            }
        }
    template <typename T>
        cnt_ptr<T>::operator bool() const { assert(invariants_()); return ptr_; }
    template <typename T>
        T *cnt_ptr<T>::operator->() const { assert(invariants_()); return ptr_; }
    template <typename T>
        T &cnt_ptr<T>::operator*() const
        {
            assert(invariants_());
            assert(ptr_);
            return *ptr_;
        }
    template <typename T>
        bool cnt_ptr<T>::invariants_() const
        {
            assert((ptr_ and ref_) or (!ptr_ and !ref_));
            if (ref_)
            {
                assert(ref_->share > 0);
            }
            return true;
        }
} 

#endif
