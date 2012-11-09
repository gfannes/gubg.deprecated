#ifndef HEADER_gubg_visualization_visualization_hpp_ALREADY_INCLUDED
#define HEADER_gubg_visualization_visualization_hpp_ALREADY_INCLUDED

//This needs some serious rework

#include "canvas.hpp"
#include "drawable.hpp"
#include "gubg/pointer/Locked.hpp"
#include "boost/thread.hpp"
#include <iostream>
#include <vector>
#include <ctime>
#include <sys/time.h> 

using namespace std;

namespace gubg
{
    class Visualization
    {
        private:
            // This is the function that is called before drawing each frame and should be overloaded to insert your own specific dynamics. Return false if you want to exit.
            virtual bool dynamics(){cout << "This is the base Visualization::dynamics(), please overload me" << endl; return true;}
            // This function is called when start is issued, and allows you to set things last-minute, like time (mCurrentTick is valid)
            virtual bool onStart(){cout << "This is the base Visualization::onStart(), please overload me" << endl; return true;}

            class Drawables
            {
                public:
                    Drawables():
                        mFirstIsCurrent(true){}
                    void flip(){mFirstIsCurrent = !mFirstIsCurrent;}
                    vector<Drawable*> &getCurrent(){return (mFirstIsCurrent ? mFirst : mSecond);}
                    vector<Drawable*> &get(bool current){return (current == mFirstIsCurrent ? mFirst : mSecond);}

                    vector<Drawable*> mFirst;
                    vector<Drawable*> mSecond;
                    bool mFirstIsCurrent;
            };

        public:
            Visualization(int width, int height, bool bShow = true, unsigned int bgColor = 0x123456):
                _canvas(width,height),
                _show(bShow),
                quit_(true),
                _backgroundColor(bgColor),
                drawables_(new Drawables){}
            ~Visualization()
            {
                finishThread();
            }

            int createBasis(double xO, double yO, double xW, double yW){return _canvas.createBasis(xO,yO,xW,yW);}
            bool addDrawable(int basis, Drawable *drawable, bool current = true)
            {
                if (_show)
                {
                    drawable->setBasis(basis);
                    drawables_->get(current).push_back(drawable);
                }
                return true;
            }

            void initialize();
            void start();
            void handleInput();
            void draw();
            void flip()
            {
                if (_show)
                    drawables_->flip();
            }
            int width(){return _canvas.width();}
            int height(){return _canvas.height();}
            bool clear(bool deleteMemory = false)
            {
                if (_show)
                {
                    DrawablesPtr::Unlock unlock(drawables_);
                    vector<Drawable*> &drawables = unlock->getCurrent();
                    if (deleteMemory)
                        for (int i=0;i<drawables.size();++i)
                            delete drawables[i];
                    drawables.resize(0);
                    _canvas.clear(_backgroundColor);
                }
                return true;
            }

            void startThread(){thread_ = boost::thread(boost::ref(*this));}
            void finishThread(){quit_ = true; thread_.join();}

            void operator()(){start();}

        protected:
            void updateTime(bool start = false)
            {
                if (start)
                {
                    gettimeofday(&mStartTV,0);
                    mCurrentTV = mStartTV;
                    mCurrentTick = mStartTick = (1000000*long(mStartTV.tv_sec)+long(mStartTV.tv_usec));
                    mElapsedTime = mTotalElapsedTime = 0;
                    return;
                }
                long previousTick = mCurrentTick;
                gettimeofday(&mCurrentTV,0);
                mCurrentTick = (1000000*long(mCurrentTV.tv_sec)+long(mCurrentTV.tv_usec));
                mElapsedTime = mCurrentTick - previousTick;
                mTotalElapsedTime = mCurrentTick - mStartTick;
            }
            struct timeval mStartTV;
            struct timeval mCurrentTV;
            long mStartTick;
            long mCurrentTick;
            long mElapsedTime;                // Measured in microseconds
            long mTotalElapsedTime;        // Measured in microseconds

        private:
            Canvas _canvas;
            bool _show;
            bool quit_;
            typedef gubg::pointer::Locked<Drawables, gubg::pointer::ThreadSafeInstance> DrawablesPtr;
            DrawablesPtr drawables_;
            Cairo::Matrix _FPSBasis;
            unsigned int _backgroundColor;
            boost::thread thread_;
    };
};

#endif
