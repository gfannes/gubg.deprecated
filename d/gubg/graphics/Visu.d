module gubg.graphics.Visu;

version (linux)
{
    import derelict.sdl.sdl;
    public import tango.core.Thread;

    import gubg.graphics.Scene;
    import gubg.Array;
    import gubg.Math;
    import gubg.Timer;
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
            isThreaded_ = false;
            isFinished_ = false;
            canvas_ = new SDLCanvas(width_, height_);
            scene_ = new Scene(canvas_, newOrig_, newOne_);
        }
        ~this ()
        {
            if (isThreaded_)
            {
                isFinished_ = true;
                if (thread_.isRunning())
                    thread_.join();
                thread_ = null;
            }
            if (scene_)
                delete scene_;
            if (canvas_)
                delete canvas_;
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
            timer_ = new Timer(false);

            if (isThreaded_)
            {
                thread_ = new Thread(&drawLoop_);
                thread_.start();
            } else
            {
                puts("Unthreaded");
                drawLoop_();
            }

            return true;
        }
        //Amount of time elapsed since show() in seconds
        real elapsedTime(){return timer_.difference;}

        bool isFinished()
        {
            if (isFinished_)
                return true;

            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    // exit if SDLK or the window close button are pressed
                    case SDL_KEYUP:
                        if(event.key.keysym.sym == SDLK_ESCAPE)
                            isFinished_ = true;
                        break;
                    case SDL_QUIT:
                        isFinished_ = true;
                        break;
                    default:
                        break;	
                }
            }
            return isFinished_;
        }

        IDrawable add(IDrawable drawable)
        {
            if (!scene_)
                throw new Exception("No scene to add elements to, maybe the visualization is already over");
            scene_.add(drawable);
            return drawable;
        }

        private:
        void drawLoop_()
        {
            while (true)
            {
                if (beforeDraw_ && !beforeDraw_())
                {
                    isFinished_ = true;
                    break;
                }

                scene_.draw();

                if (isFinished())
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

        bool isThreaded_;
        bool delegate() beforeDraw_;
        Thread thread_;
        ICanvas canvas_;
        Scene scene_;
        bool isFinished_;
        int width_;
        int height_;
        real[] newOrig_;
        real[] newOne_;
        Timer timer_;
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
            auto lf = new Factory(null, null);	// Factory used for creating lines, we use no predefined stroke nor fill style
            auto of = new Factory(null, null);	// Factory used for creating squares and circles, we use no predefined stroke nor fill style

            {
                scope visu = new Visu(640, 480);

                // Delegate that will be called within the draw loop
                bool printer()
                {
                    puts("elapsed time = {}", visu.elapsedTime());
                    return true;
                }

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

                // Start the visualization thread
                visu.show(true, &printer, 0.1);

                puts("Going to sleep");
                Thread.sleep(2);
                puts("Woken up");
            }
        }
    }
}
