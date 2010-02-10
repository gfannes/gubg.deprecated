module gubg.graphics.Visu;

version (linux)
{
    import derelict.sdl.sdl;
    import tango.core.Thread;
    import tango.time.StopWatch;

    import gubg.graphics.Scene;
    import gubg.Array;
    import gubg.Math;
    public import gubg.graphics.Drawable;
    public import gubg.graphics.Style;

    import gubg.Puts;

    class Visu
    {
        this (int width, int height, real[] newOrig = null, real[] newOne = null)
        {
            width_ = width;
            height_ = height;
            newOrig_.assign(newOrig);
            newOne_.assign(newOne);
        }
        static Visu create(int width, int height, real[2] minMaxX, real[2] minMaxY)
        {
            real[2] ltX, ltY;
            computeLinTrans(ltX, minMaxX, [0, width-1]);
            computeLinTrans(ltY, minMaxY, [0, height-1]);
            auto visu = new Visu(width, height, [transformLinTrans(0.0, ltX), transformLinTrans(0.0, ltY)], [transformLinTrans(1.0, ltX), transformLinTrans(1.0, ltY)]);
            return visu;
        }

        bool show(bool threaded = true, bool delegate() beforeDraw = null, real sleepTime = 0.01)
        {
            isThreaded_ = threaded;
            beforeDraw_ = beforeDraw;
            sleepTime_ = sleepTime;
            canvas_ = new SDLCanvas(width_, height_);
            scene_ = new Scene(canvas_, newOrig_, newOne_);
            quit_ = false;
            stopwatch_.start();

            if (isThreaded_)
            {
                thread_ = new Thread(&drawLoop);
                thread_.start();
            } else
            {
                puts("Unthreaded");
                drawLoop();
            }

            return true;
        }
        void drawLoop()
        {
            while (true)
            {
                if (beforeDraw_ && !beforeDraw_())
                    break;

                scene_.draw();

                if (quit())
                {
                    puts("Quitting Visu...");
                    break;
                }

                if (sleepTime_ > 0)
                    Thread.sleep(sleepTime_);
            }
            delete scene_;
            scene_ = null;
            delete canvas_;
            canvas_ = null;
        }

        // The number of microseconds elapsed since show()
        ulong elapsedTime(){return stopwatch_.microsec();}

        void stop()
        {
            if (isThreaded_)
            {
                quit_ = true;
                if (thread_.isRunning())
                    thread_.join();
                thread_ = null;
            }
        }

        bool quit()
        {
            if (quit_)
                return true;

            bool bRet = false;
            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    // exit if SDLK or the window close button are pressed
                    case SDL_KEYUP:
                        if(event.key.keysym.sym == SDLK_ESCAPE)
                            bRet = true;
                        break;
                    case SDL_QUIT:
                        bRet = true;
                        break;
                    default:
                        break;	
                }
            }
            return bRet;
        }

        IDrawable add(IDrawable drawable)
        {
            scene_.add(drawable);
            return drawable;
        }

        private:
        bool isThreaded_;
        bool delegate() beforeDraw_;
        Thread thread_;
        ICanvas canvas_;
        Scene scene_;
        bool quit_;
        int width_;
        int height_;
        real[] newOrig_;
        real[] newOne_;
        StopWatch stopwatch_;
        real sleepTime_;
    }

    class Factory
    {
        this(StrokeStyle strokeStyle = StrokeStyle.standard, FillStyle fillStyle = FillStyle.standard)
        {
            strokeStyle_ = strokeStyle;
            fillStyle_ = fillStyle;
        }

        void strokeColor(Color color)
        {
            strokeStyle_ = strokeStyle_ ? strokeStyle_.dup : StrokeStyle.standard;
            strokeStyle_.strokeColor(color);
        }
        void strokeWidth(real width)
        {
            strokeStyle_ = strokeStyle_ ? strokeStyle_.dup : StrokeStyle.standard;
            strokeStyle_.strokeWidth(width);
        }
        void fillColor(Color color)
        {
            fillStyle_ = fillStyle_ ? fillStyle_.dup : FillStyle.standard;
            fillStyle_.fillColor(color);
        }

        Line createLine(real[] sco, real[] eco){return new Line(sco, eco, strokeStyle_);}
        Path createPath(real[] xcos, real[] ycos)
        {
            return new Path(xcos, ycos, strokeStyle_);
        }
        Circle createCircle(real[] center, real radius){return new Circle(center, radius, strokeStyle_, fillStyle_);}
        Rectangle createRectangle(real[] lb, real[] tr){return new Rectangle(lb, tr, strokeStyle_, fillStyle_);}
        Rectangle createCenteredRectangle(real[] center, real[] wh){return createRectangle([center[0]-0.5*wh[0], center[1]-0.5*wh[1]], [center[0]+0.5*wh[0], center[1]+0.5*wh[1]]);}
        Rectangle createCenteredSquare(real[] center, real side){return createRectangle([center[0]-0.5*side, center[1]-0.5*side], [center[0]+0.5*side, center[1]+0.5*side]);}

        private:
        StrokeStyle strokeStyle_;
        FillStyle fillStyle_;
    }

    version(UnitTest)
    {
        void main()
        {
            auto visu = new Visu(640, 480);
            auto lf = new Factory(null, null);	// Factory used for creating lines, we use no predefined stroke nor fill style
            auto of = new Factory(null, null);	// Factory used for creating squares and circles, we use no predefined stroke nor fill style

            // Delegate that will be called within the draw loop
            bool printer()
            {
                puts("elapsed time = {}", visu.elapsedTime());
                return true;
            }

            // Start the visualization thread
            visu.show(true, &printer, 0.1);

            // Create the graphical objects with our two factories
            for (real x = -1.0; x<=1.05; x+=0.1)
            {
                for (real y = -1.0; y<=1.05; y+=0.1)
                {
                    of.fillColor(Color(0.5*(-x+1), 0.5*(-y+1), 0.25*(-x-y+2)));
                    visu.add(of.createCircle([x, y], 0.03));
                    visu.add(of.createCenteredSquare([x, y], 0.06));

                    lf.strokeColor(Color(0.5*(x+1), 0.5*(y+1), 0.25*(x+y+2)));
                    visu.add(lf.createLine([x-0.03, y-0.03], [x+0.03, y+0.03]));
                }
            }

            puts("Going to sleep");
            Thread.sleep(2);
            puts("Woken up");

            visu.stop();
        }
    }
}
