module gubg.canvas;

import gubg.style;
import gubg.puts;

interface Canvas
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
    bool drawCircle(real[] center, real radius);
    bool fillCircle(real[] center, real radius);
    bool drawRectangle(real[] lbco, real[] trco);
    bool fillRectangle(real[] lbco, real[] trco);

    // Style settings
    bool setStrokeStyle(Style style);
    bool setFillStyle(Style style);
}

class ConsoleCanvas: Canvas
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
    bool drawCircle(real[] center, real radius)
    {
	puts("Drawing circle with center {} and radius {}", center, radius);
	return true;
    }
    bool fillCircle(real[] center, real radius)
    {
	puts("Filling circle with center {} and radius {}", center, radius);
	return true;
    }
    bool drawRectangle(real[] lbco, real[] trco){return true;}
    bool fillRectangle(real[] lbco, real[] trco){return true;}

    bool setStrokeStyle(Style style){return true;}
    bool setFillStyle(Style style){return true;}
}

import derelict.sdl.sdl;
import cairo.ImageSurface;
import cairo.Context;
import gtkc.cairotypes;

class SDLCanvas: Canvas
{
    this(int width, int height)
    {
	mWidth = width;
	mHeight = height;
	mInitialized = false;
	mSDLSurface = null;
    }

    bool initialize()
    {
	if (mInitialized)
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
	mSDLSurface = SDL_SetVideoMode(mWidth, mHeight, 32, SDL_SWSURFACE);
	if (mSDLSurface is null)
	{
	    err("Unable to set {} x {} video: {}\n", mWidth, mHeight, SDL_GetError());
	    SDL_Quit();
	    return false;
	}

	mCairoSurface = ImageSurface.createForData(cast(ubyte*)(mSDLSurface.pixels), cairo_format_t.ARGB32, mWidth, mHeight, 4*mWidth);
	mCairoContext = Context.create(mCairoSurface);

	mInitialized = true;
	return true;
    }
    bool finalize()
    {
	if (mInitialized)
	    SDL_Quit();
	return true;
    }

    bool initializeDraw()
    {
	if (SDL_MUSTLOCK(mSDLSurface))
	    if (SDL_LockSurface(mSDLSurface) < 0)
		return false;
	clear();
	return true;
    }
    void clear(uint rgb = 0x123456){SDL_FillRect( mSDLSurface, null, rgb);}

    void finalizeDraw()
    {
	if (SDL_MUSTLOCK(mSDLSurface))
	    SDL_UnlockSurface(mSDLSurface);
	SDL_UpdateRect(mSDLSurface, 0, 0, mWidth, mHeight);
    }
    void flip()
    {
	SDL_Flip(mSDLSurface);
    }

    int width(){return mWidth;}
    int height(){return mHeight;}

    bool drawLine(real[] sco, real[] eco)
    {
	mCairoContext.moveTo(sco[0], mHeight-1-sco[1]);
	mCairoContext.lineTo(eco[0], mHeight-1-eco[1]);
	mCairoContext.stroke();
	return true;
    }
    bool drawCircle(real[] center, real radius)
    {
	mCairoContext.arc(center[0], mHeight-1-center[1], radius, 0.0, 6.28318530717959);
	mCairoContext.stroke();
	return true;
    }
    bool fillCircle(real[] center, real radius)
    {
	mCairoContext.arc(center[0], mHeight-1-center[1], radius, 0.0, 6.28318530717959);
	mCairoContext.fill();
	return true;
    }
    bool drawRectangle(real[] lbco, real[] trco)
    {
	mCairoContext.moveTo(lbco[0], mHeight-1-lbco[1]);
	mCairoContext.lineTo(lbco[0], mHeight-1-trco[1]);
	mCairoContext.lineTo(trco[0], mHeight-1-trco[1]);
	mCairoContext.lineTo(trco[0], mHeight-1-lbco[1]);
	mCairoContext.lineTo(lbco[0], mHeight-1-lbco[1]);
	mCairoContext.stroke();
	return true;
    }
    bool fillRectangle(real[] lbco, real[] trco)
    {
	mCairoContext.moveTo(lbco[0], mHeight-1-lbco[1]);
	mCairoContext.lineTo(lbco[0], mHeight-1-trco[1]);
	mCairoContext.lineTo(trco[0], mHeight-1-trco[1]);
	mCairoContext.lineTo(trco[0], mHeight-1-lbco[1]);
	mCairoContext.lineTo(lbco[0], mHeight-1-lbco[1]);
	mCairoContext.fill();
	return true;
    }


    bool setStrokeStyle(Style style)
    {
	mCairoContext.setLineWidth(style.strokeWidth);
	mCairoContext.setSourceRgb(style.strokeColor().red, style.strokeColor().green, style.strokeColor().blue);
	return true;
    }
    bool setFillStyle(Style style)
    {
	mCairoContext.setSourceRgb(style.fillColor().red, style.fillColor().green, style.fillColor().blue);
	return true;
    }

private:
    SDL_Surface* mSDLSurface;
    ImageSurface mCairoSurface;
    Context mCairoContext;
    
    int mWidth;
    int mHeight;
    bool mInitialized;
}

version(UnitTest)
{
    void main()
    {
	ConsoleCanvas canvas = new ConsoleCanvas;
	canvas.drawLine([1,2], [3,4]);
	canvas.drawCircle([5,6], 7);
    }
}