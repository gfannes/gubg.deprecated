module gubg.graphics.Canvas;

public import gubg.graphics.Style;

import derelict.sdl.sdl;
import Cairo = gubg.graphics.Cairo;

import gubg.Puts;

interface ICanvas
{
    // Called once or more before any other call
    bool initialize();
    bool finalize();

    // Called before and after each draw session
    bool initializeDraw();
    void finalizeDraw();

    // width and height of canvas
    // (0,0) is bottom left and (width-1, height-1) is top right
    int width();
    int height();

    // Drawing primitives
    void drawLine(real[] sco, real[] eco, StrokeStyle);
    void drawCircle(real[] center, real radius, StrokeStyle, FillStyle);
    void drawRectangle(real[] lbco, real[] trco, StrokeStyle, FillStyle);
    // TODO
    // void drawPixel();
    // void drawPath();
}

class ConsoleCanvas: ICanvas
{
public:				// ICanvas
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

    void drawLine(real[] sco, real[] eco, StrokeStyle strokeStyle)
    {
	puts("Drawing line from {} to {} in style {}", sco, eco, strokeStyle);
    }
    void drawCircle(real[] center, real radius, StrokeStyle strokeStyle, FillStyle fillStyle)
    {
	puts("Drawing circle with center {} and radius {}, strokeStyle {}, fillStyle {}", center, radius, strokeStyle, fillStyle);
    }
    void drawRectangle(real[] lbco, real[] trco, StrokeStyle strokeStyle, FillStyle fillStyle)
    {
	puts("Drawing rectangle from left bottom {} to top right {}, strokeStyle {}, fillStyle {}", lbco, trco, strokeStyle, fillStyle);
    }
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

public: 			// ICanvas
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
    void finalizeDraw()
    {
	if (SDL_MUSTLOCK(_SDLSurface))
	    SDL_UnlockSurface(_SDLSurface);
	SDL_UpdateRect(_SDLSurface, 0, 0, _width, _height);
    }

    int width(){return _width;}
    int height(){return _height;}

    void drawLine(real[] sco, real[] eco, StrokeStyle strokeStyle)
    {
	if (strokeStyle)
	{
	    setStrokeStyle(strokeStyle);
	    _cairo.moveTo(sco[0], _height-1-sco[1]);
	    _cairo.lineTo(eco[0], _height-1-eco[1]);
	    _cairo.stroke();
	}
    }
    void drawCircle(real[] center, real radius, StrokeStyle strokeStyle, FillStyle fillStyle)
    {
	setStrokeStyle(strokeStyle);
	setFillStyle(fillStyle);
        _cairo.arc(center[0], _height-1-center[1], radius, 0.0, 6.28318530717959);
	// First fill, then stroke
	if (fillStyle)
	    _cairo.fill();
	if (strokeStyle)
	    _cairo.stroke();
    }
    void drawRectangle(real[] lbco, real[] trco, StrokeStyle strokeStyle, FillStyle fillStyle)
    {
	setStrokeStyle(strokeStyle);
	setFillStyle(fillStyle);
        _cairo.moveTo(lbco[0], _height-1-lbco[1]);
        _cairo.lineTo(lbco[0], _height-1-trco[1]);
        _cairo.lineTo(trco[0], _height-1-trco[1]);
        _cairo.lineTo(trco[0], _height-1-lbco[1]);
        _cairo.lineTo(lbco[0], _height-1-lbco[1]);
	// First fill, then stroke
	if (fillStyle)
	    _cairo.fill();
	if (strokeStyle)
	    _cairo.stroke();
    }

private:			// Methods
    void clear(uint rgb = 0x123456){SDL_FillRect( _SDLSurface, null, rgb);}
    void flip(){SDL_Flip(_SDLSurface);}
    void setStrokeStyle(StrokeStyle strokeStyle)
    {
	if (strokeStyle)
	{
	    _cairo.setLineWidth(strokeStyle.strokeWidth);
	    _cairo.setSourceRGB(strokeStyle.strokeColor.red, strokeStyle.strokeColor.green, strokeStyle.strokeColor.blue);
	}
    }
    void setFillStyle(FillStyle fillStyle)
    {
	if (fillStyle)
	    _cairo.setSourceRGB(fillStyle.fillColor.red, fillStyle.fillColor.green, fillStyle.fillColor.blue);
    }

private:			// Data
    SDL_Surface* _SDLSurface;
    Cairo.Context _cairo;
    
    int _width;
    int _height;
    bool _initialized;
}

version(UnitTest)
{
    import tango.core.Thread;
    import gubg.Puts;

    void main()
    {
 	auto canvas = new ConsoleCanvas;
//	auto canvas = new SDLCanvas(100, 100);
	canvas.initialize;
	canvas.initializeDraw;
	puts("Before drawLine");
 	canvas.drawLine([1.0, 2], [90.0, 90], StrokeStyle.standard);
	puts("After drawLine");
// 	canvas.flip;
// 	Thread.sleep(3);
	canvas.finalize;
	puts("Finished");
    }
}
