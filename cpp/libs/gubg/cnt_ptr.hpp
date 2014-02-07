#ifndef HEADER_gubg_cnt_ptr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cnt_ptr_hpp_ALREADY_INCLUDED

//!@#adoc

//cnt_ptr is meant as a non-thread-safe alternative to std::shared_ptr, but in its current implementation.
//track_ptr is meant to be the weak_ptr equivalent, but it is not implemented yet, nor is enable_cnt_from_this

//They should also consume less memory, and be faster, but real tests are needed, and this implementation
//is still very alpha:
//* no casting support
//* enable_cnt_from_this is not yet implemented

#include <cassert>

namespace gubg { 

    namespace priv { 
        template <typename T>
            struct Ref
            {
                //Is dangling if cnt == 0 (and track > 0)
                T &obj_;
                //For speed, we assume you want to set the ref count to 1 initially
                typedef unsigned int Count;
                Count cnt = 1;
                Count track  = 0;

                Ref(T &p): obj_(p) {}
            };
    } 

    template <typename T>
        class track_ptr;

    template <typename T>
        class cnt_ptr
        {
            public:
                cnt_ptr();
                explicit cnt_ptr(T *);
                cnt_ptr(const cnt_ptr<T> &);
                cnt_ptr &operator=(const cnt_ptr<T> &);

                ~cnt_ptr();

                bool operator==(const cnt_ptr<T> &) const;

                void reset();
                void reset(T *);

                explicit operator bool() const;

                T *operator->() const;
                T &operator*() const;
                T *get() const;

            private:
                bool invariants_() const;
                //Does not respect invariants_() for speed
                void decrement_();

                friend class track_ptr<T>;
                typedef priv::Ref<T> Ref;
                Ref *ref_ = nullptr;
        };

    template <typename T>
        class track_ptr
        {
            public:
                track_ptr();
                explicit track_ptr(const cnt_ptr<T> &);
                track_ptr &operator=(const track_ptr<T> &);
                track_ptr &operator=(const cnt_ptr<T> &);

                ~track_ptr();

                cnt_ptr<T> lock() const;

                void reset();

            private:
                bool invariants_() const;
                void decrement_();

                typedef priv::Ref<T> Ref;
                Ref *ref_ = nullptr;
        };

    //Implementation of cnt_ptr
    template <typename T>
        cnt_ptr<T>::cnt_ptr() { assert(invariants_()); }
    template <typename T>
        cnt_ptr<T>::cnt_ptr(T *p): ref_(p ? new Ref(*p) : nullptr) { assert(invariants_()); }
    template <typename T>
        cnt_ptr<T>::cnt_ptr(const cnt_ptr<T> &rhs): ref_(rhs.ref_)
    {
        assert(rhs.invariants_());
        if (ref_)
            ++ref_->cnt;
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
                ref_ = rhs.ref_;
                if (ref_)
                    ++ref_->cnt;
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
        bool cnt_ptr<T>::operator==(const cnt_ptr<T> &rhs) const
        {
            assert(invariants_());
            assert(rhs.invariants_());
            return ref_ == rhs.ref_;
        }
    template <typename T>
        void cnt_ptr<T>::reset()
        {
            assert(invariants_());
            decrement_();
            ref_ = nullptr;
            assert(invariants_());
        }
    template <typename T>
        void cnt_ptr<T>::reset(T *p)
        {
            assert(invariants_());
            decrement_();
            if (p)
                ref_ = new Ref(*p);
            else
                ref_ = nullptr;
            assert(invariants_());
        }
    template <typename T>
        void cnt_ptr<T>::decrement_()
        {
            if (ref_)
            {
                if (ref_->cnt == 1)
                {
                    //A bit dangerous, this is dangling now
                    delete &ref_->obj_;
                    if (ref_->track == 0)
                        delete ref_;
                    else
                        ref_->cnt = 0;
                }
                else
                    --ref_->cnt;
            }
        }
    template <typename T>
        cnt_ptr<T>::operator bool() const { assert(invariants_()); return ref_; }
    template <typename T>
        T *cnt_ptr<T>::operator->() const { assert(invariants_()); return &ref_->obj_; }
    template <typename T>
        T &cnt_ptr<T>::operator*() const
        {
            assert(invariants_());
            assert(ref_);
            return ref_->obj_;
        }
    template <typename T>
        T *cnt_ptr<T>::get() const
        {
            assert(invariants_());
            if (!ref_)
                return nullptr;
            return &ref_->obj_;
        }
    template <typename T>
        bool cnt_ptr<T>::invariants_() const
        {
            if (ref_)
            {
                assert(ref_->cnt > 0);
            }
            return true;
        }

    //Implementation of track_ptr
    template <typename T>
        track_ptr<T>::track_ptr() { assert(invariants_()); }
    template <typename T>
        track_ptr<T>::track_ptr(const cnt_ptr<T> &rhs): ref_(rhs.ref_)
    {
        assert(rhs.invariants_());
        if (ref_)
            ++ref_->track;
        assert(invariants_());
    }
    template <typename T>
        track_ptr<T> & track_ptr<T>::operator=(const track_ptr<T> &rhs)
        {
            assert(invariants_());
            assert(rhs.invariants_());
            if (&rhs != this)
            {
                decrement_();
                ref_ = rhs.ref_;
                if (ref_)
                    ++ref_->track;
            }
            assert(invariants_());
            return *this;
        }
    template <typename T>
        track_ptr<T> & track_ptr<T>::operator=(const cnt_ptr<T> &rhs)
        {
            assert(invariants_());
            assert(rhs.invariants_());
            decrement_();
            ref_ = rhs.ref_;
            if (ref_)
                ++ref_->track;
            assert(invariants_());
            return *this;
        }
    template <typename T>
        track_ptr<T>::~track_ptr()
        {
            assert(invariants_());
            decrement_();
        }
    template <typename T>
        cnt_ptr<T> track_ptr<T>::lock() const
        {
            assert(invariants_());
            cnt_ptr<T> ret;
            if (ref_ and ref_->cnt > 0)
            {
                ret.ref_ = ref_;
                ++ref_->cnt;
            }
            return ret;
        }
    template <typename T>
        void track_ptr<T>::reset()
        {
            assert(invariants_());
            decrement_();
            ref_ = nullptr;
            assert(invariants_());
        }
    template <typename T>
        void track_ptr<T>::decrement_()
        {
            if (ref_)
            {
                if (ref_->track == 1)
                {
                    if (ref_->cnt == 0)
                        delete ref_;
                    else
                        ref_->track = 0;
                }
                else
                    --ref_->track;
            }
        }
    template <typename T>
        bool track_ptr<T>::invariants_() const
        {
            if (ref_)
            {
                assert(ref_->track > 0);
            }
            return true;
        }
} 

#endif
