module gubg.graphics.canvas;

import gubg.graphics.style;

import derelict.sdl.sdl;
import Cairo = gubg.graphics.cairo;

import gubg.puts;

interface ICanvas
{
    // Called once or more before any other call
    bool initialize();
    bool finalize();

    // Called before and after each draw session
    bool initializeDraw();
    void finalizeDraw();

    // width and height of canvas
    // (0,0) is bottom left and (width()-1, height()-1) is top right
    int width();
    int height();

    // Drawing primitives
    bool drawLine(real[] sco, real[] eco);
    bool drawCircle(real[] center, real radius, bool fill);
    bool drawRectangle(real[] lbco, real[] trco, bool fill);

    // Style settings
    bool setStrokeStyle(Style style);
    bool setFillStyle(Style style);
}

class ConsoleCanvas: ICanvas
{
    bool initialize()
    {
	puts("Initializing ConsoleCanvas");
	return true;
    }
    bool finalize(){return true;}
    bool initializeDraw()
    {
	puts("Initializing ConsoleCanvas for drawing");
	return true;
    }
    void finalizeDraw(){puts("Finalizing ConsoleCanvas for drawing");}

    int width(){return -1;}
    int height(){return -1;}

    bool drawLine(real[] sco, real[] eco)
    {
	puts("Drawing line from {} to {}", sco, eco);
	return true;
    }
    bool drawCircle(real[] center, real radius, bool fill)
    {
	puts("Drawing circle with center {} and radius {}, fill {}", center, radius, fill);
	return true;
    }
    bool drawRectangle(real[] lbco, real[] trco, bool fill){return true;}

    bool setStrokeStyle(Style style){return true;}
    bool setFillStyle(Style style){return true;}
}

class SDLCanvas: ICanvas
{
    this(int width, int height)
    {
	_width = width;
	_height = height;
	_initialized = false;
	_SDLSurface = null;
    }

    bool initialize()
    {
	if (_initialized)
	    return true;

	DerelictSDL.load();
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
	    err("Unable to init SDL: {}\n", SDL_GetError());
	    SDL_Quit();
	    return false;
	}
	// Create the screen surface (window)
	_SDLSurface = SDL_SetVideoMode(_width, _height, 32, SDL_SWSURFACE);
	if (_SDLSurface is null)
	{
	    err("Unable to set {} x {} video: {}\n", _width, _height, SDL_GetError());
	    SDL_Quit();
	    return false;
	}

	_cairo = new Cairo.Context(cast(ubyte*)(_SDLSurface.pixels), _width, _height);

	_initialized = true;
	return true;
    }
    bool finalize()
    {
	if (_initialized)
	    SDL_Quit();
	return true;
    }

    bool initializeDraw()
    {
	if (SDL_MUSTLOCK(_SDLSurface))
	    if (SDL_LockSurface(_SDLSurface) < 0)
		return false;
	clear();
	return true;
    }
    void clear(uint rgb = 0x123456){SDL_FillRect( _SDLSurface, null, rgb);}

    void finalizeDraw()
    {
	if (SDL_MUSTLOCK(_SDLSurface))
	    SDL_UnlockSurface(_SDLSurface);
	SDL_UpdateRect(_SDLSurface, 0, 0, _width, _height);
    }
    void flip()
    {
	SDL_Flip(_SDLSurface);
    }

    int width(){return _width;}
    int height(){return _height;}

    bool drawLine(real[] sco, real[] eco)
    {
        _cairo.moveTo(sco[0], _height-1-sco[1]);
        _cairo.lineTo(eco[0], _height-1-eco[1]);
        _cairo.stroke();
	return true;
    }
    bool drawCircle(real[] center, real radius, bool fill)
    {
        _cairo.arc(center[0], _height-1-center[1], radius, 0.0, 6.28318530717959);
	if (fill)
	    _cairo.fill();
	else
	    _cairo.stroke();
	return true;
    }
    bool drawRectangle(real[] lbco, real[] trco, bool fill)
    {
        _cairo.moveTo(lbco[0], _height-1-lbco[1]);
        _cairo.lineTo(lbco[0], _height-1-trco[1]);
        _cairo.lineTo(trco[0], _height-1-trco[1]);
        _cairo.lineTo(trco[0], _height-1-lbco[1]);
        _cairo.lineTo(lbco[0], _height-1-lbco[1]);
	if (fill)
	    _cairo.fill();
	else
	    _cairo.stroke();
	return true;
    }

    bool setStrokeStyle(Style style)
    {
        _cairo.setLineWidth(style.strokeWidth);
        _cairo.setSourceRGB(style.strokeColor().red, style.strokeColor().green, style.strokeColor().blue);
	return true;
    }
    bool setFillStyle(Style style)
    {
        _cairo.setSourceRGB(style.fillColor().red, style.fillColor().green, style.fillColor().blue);
	return true;
    }

private:
    SDL_Surface* _SDLSurface;
    Cairo.Context _cairo;
    
    int _width;
    int _height;
    bool _initialized;
}

version(UnitTest)
{
    import tango.core.Thread;
    import gubg.puts;

    void main()
    {
// 	auto canvas = new ConsoleCanvas;
	auto canvas = new SDLCanvas(100, 100);
	canvas.initialize;
	canvas.clear;
	canvas.setStrokeStyle(Style.defaultStyle);
	puts("Before drawLine");
 	canvas.drawLine([1, 2], [90, 90]);
	puts("After drawLine");
	canvas.fillCircle([5,6], 7);
	canvas.flip;
	Thread.sleep(3);
	canvas.finalize;
	puts("Finished");
    }
}
