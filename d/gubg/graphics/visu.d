module gubg.graphics.visu;

import derelict.sdl.sdl;
import tango.core.Thread;
import tango.time.StopWatch;

import gubg.graphics.scene;
import gubg.graphics.canvas;
public import gubg.graphics.drawable;
public import gubg.graphics.style;
import gubg.puts;

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

    Drawable addLine(real[] sco, real[] eco)
	{
	    Drawable line = new Line(sco, eco);
	    _scene.add(line);
	    return line;
	}

    Drawable addCircle(real[] center, real radius)
	{
	    Drawable circle = new Circle(center, radius);
	    _scene.add(circle);
	    return circle;
	}

    Rectangle addRectangle(real[] lb, real[] tr)
	{
	    auto rectangle = new Rectangle(lb, tr);
	    _scene.add(rectangle);
	    return rectangle;
	}
    Drawable addCenteredRectangle(real[] center, real[] wh){return addRectangle([center[0]-0.5*wh[0], center[1]-0.5*wh[1]], [center[0]+0.5*wh[0], center[1]+0.5*wh[1]]);}
    Drawable addCenteredSquare(real[] center, real side){return addRectangle([center[0]-0.5*side, center[1]-0.5*side], [center[0]+0.5*side, center[1]+0.5*side]);}

    void setStroke(bool stroke)
	{
	    _scene.currentStyle().setStroke(stroke);
	}
    void setStrokeColor(Color color)
	{
	    _scene.currentStyle().setStroke(true);
	    _scene.currentStyle().setStrokeColor(color);
	}

    void setFill(bool fill)
	{
	    _scene.currentStyle().setFill(fill);
	}
    void setFillColor(Color color)
	{
	    _scene.currentStyle().setFill(true);
	    _scene.currentStyle().setFillColor(color);
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

version(UnitTest)
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
//		visu.setStrokeColor(Color(0.5*(x+1), 0.5*(y+1), 0.25*(x+y+2)));
		visu.setFillColor(Color(0.5*(-x+1), 0.5*(-y+1), 0.25*(-x-y+2)));
//		visu.addCircle([x, y], 0.03);
		visu.addRectangle([x-0.02, y-0.02], [x+0.02, y+0.02]);
	    }
	}
	Thread.sleep(25);
	puts("Woken up");
	visu.stop();
    }
}
