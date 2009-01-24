module gubg.visu;

import derelict.sdl.sdl;
import tango.core.Thread;
import tango.time.StopWatch;

import gubg.scene;
import gubg.canvas;
import gubg.drawable;
import gubg.style;
import gubg.puts;

class Visu
{
    this (int width, int height)
	{
	    mWidth = width;
	    mHeight = height;

	    mCanvas = new SDLCanvas(mWidth, mHeight);
	    mScene = new Scene(mCanvas);
	    mQuit = false;
	}

    bool show(bool threaded = true, bool delegate() beforeDraw = null, real sleepTime = 0.01)
	{
	    mThreaded = threaded;
	    mBeforeDraw = beforeDraw;
	    mSleepTime = sleepTime;
	    mStopWatch.start();

	    if (mThreaded)
	    {
		mThread = new Thread(&drawLoop);
		mThread.start();
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
		if (mBeforeDraw && !mBeforeDraw())
		    break;

		mScene.draw();

		if (quit())
		{
		    puts("Quitting...");
		    break;
		}

		if (mSleepTime > 0)
		    Thread.sleep(mSleepTime);
	    }
	}

    // The number of microseconds elapsed since show()
    ulong elapsedTime(){return mStopWatch.microsec();}

    void stop()
	{
	    if (mThreaded)
	    {
		mQuit = true;
		if (mThread.isRunning())
		    mThread.join();
		mThread = null;
	    }
	}

    bool quit()
	{
	    if (mQuit)
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

    Drawable addLine(real[] sco, real[] eco)
	{
	    Drawable line = new Line(sco, eco);
	    mScene.add(line);
	    return line;
	}

    Drawable addCircle(real[] center, real radius)
	{
	    Drawable circle = new Circle(center, radius);
	    mScene.add(circle);
	    return circle;
	}

    Drawable addRectangle(real[] lb, real[] tr)
	{
	    Drawable rectangle = new Rectangle(lb, tr);
	    mScene.add(rectangle);
	    return rectangle;
	}
    Drawable addCenteredRectangle(real[] center, real[] wh){return addRectangle([center[0]-0.5*wh[0], center[1]-0.5*wh[1]], [center[0]+0.5*wh[0], center[1]+0.5*wh[1]]);}
    Drawable addCenteredSquare(real[] center, real side){return addRectangle([center[0]-0.5*side, center[1]-0.5*side], [center[0]+0.5*side, center[1]+0.5*side]);}

    void setStroke(bool stroke)
	{
	    mScene.currentStyle().setStroke(stroke);
	}
    void setStrokeColor(Color color)
	{
	    mScene.currentStyle().setStrokeColor(color);
	}

    void setFillColor(Color color)
	{
	    mScene.currentStyle().setFill(true);
	    mScene.currentStyle().setFillColor(color);
	}

private:
    bool mThreaded;
    bool delegate() mBeforeDraw;
    Thread mThread;
    Canvas mCanvas;
    Scene mScene;
    bool mQuit;
    int mWidth;
    int mHeight;
    StopWatch mStopWatch;
    real mSleepTime;
}

version(Test)
{
    void main()
    {
	Visu visu = new Visu(640, 480);
	bool printer()
	{
	    puts("elapsed time = {}", visu.elapsedTime());
	    return true;
	}
	visu.show(true, &printer, 0.1);
	puts("Going to sleep");
	visu.setStroke(false);
	for (real x = -1.0; x<=1.05; x+=0.1)
	{
	    for (real y = -1.0; y<=1.05; y+=0.1)
	    {
		visu.setStrokeColor(new Color(0.5*(x+1), 0.5*(y+1), 0.25*(x+y+2)));
		visu.setFillColor(new Color(0.5*(-x+1), 0.5*(-y+1), 0.25*(-x-y+2)));
//		visu.addCircle([x, y], 0.03);
		visu.addRectangle([x-0.02, y-0.02], [x+0.02, y+0.02]);
	    }
	}
	Thread.sleep(5);
	puts("Woken up");
	visu.stop();
    }
}
