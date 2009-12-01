module gubg.graphics.Visu;

version (linux)
{
import derelict.sdl.sdl;
import tango.core.Thread;
import tango.time.StopWatch;

import gubg.graphics.Scene;
public import gubg.graphics.Drawable;

import gubg.Puts;

class Visu
{
    this (int width, int height, real[] newOrig = null, real[] newOne = null)
	{
	    _width = width;
	    _height = height;

	    _canvas = new SDLCanvas(_width, _height);
	    _scene = new Scene(_canvas, newOrig, newOne);
	    _quit = false;
	}

    bool show(bool threaded = true, bool delegate() beforeDraw = null, real sleepTime = 0.01)
	{
	    _threaded = threaded;
	    _beforeDraw = beforeDraw;
	    _sleepTime = sleepTime;
	    _stopWatch.start();

	    if (_threaded)
	    {
		_thread = new Thread(&drawLoop);
		_thread.start();
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
		if (_beforeDraw && !_beforeDraw())
		    break;

		_scene.draw();

		if (quit())
		{
		    puts("Quitting...");
		    break;
		}

		if (_sleepTime > 0)
		    Thread.sleep(_sleepTime);
	    }
	}

    // The number of microseconds elapsed since show()
    ulong elapsedTime(){return _stopWatch.microsec();}

    void stop()
	{
	    if (_threaded)
	    {
		_quit = true;
		if (_thread.isRunning())
		    _thread.join();
		_thread = null;
	    }
	}

    bool quit()
	{
	    if (_quit)
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
	    _scene.add(drawable);
	    return drawable;
	}

private:
    bool _threaded;
    bool delegate() _beforeDraw;
    Thread _thread;
    ICanvas _canvas;
    Scene _scene;
    bool _quit;
    int _width;
    int _height;
    StopWatch _stopWatch;
    real _sleepTime;
}

class Factory
{
    this(StrokeStyle strokeStyle = StrokeStyle.standard, FillStyle fillStyle = FillStyle.standard)
	{
	    _strokeStyle = strokeStyle;
	    _fillStyle = fillStyle;
	}

    void strokeColor(Color color)
	{
	    _strokeStyle = _strokeStyle ? _strokeStyle.dup : StrokeStyle.standard;
	    _strokeStyle.strokeColor(color);
	}
    void strokeWidth(real width)
	{
	    _strokeStyle = _strokeStyle ? _strokeStyle.dup : StrokeStyle.standard;
	    _strokeStyle.strokeWidth(width);
	}
    void fillColor(Color color)
	{
	    _fillStyle = _fillStyle ? _fillStyle.dup : FillStyle.standard;
	    _fillStyle.fillColor(color);
	}

    Line createLine(real[] sco, real[] eco){return new Line(sco, eco, _strokeStyle);}
    Circle createCircle(real[] center, real radius){return new Circle(center, radius, _strokeStyle, _fillStyle);}
    Rectangle createRectangle(real[] lb, real[] tr){return new Rectangle(lb, tr, _strokeStyle, _fillStyle);}
    Rectangle createCenteredRectangle(real[] center, real[] wh){return createRectangle([center[0]-0.5*wh[0], center[1]-0.5*wh[1]], [center[0]+0.5*wh[0], center[1]+0.5*wh[1]]);}
    Rectangle createCenteredSquare(real[] center, real side){return createRectangle([center[0]-0.5*side, center[1]-0.5*side], [center[0]+0.5*side, center[1]+0.5*side]);}
	
private:
    StrokeStyle _strokeStyle;
    FillStyle _fillStyle;
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
