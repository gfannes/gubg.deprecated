module gubg.scene;

import tango.core.sync.Mutex;

import gubg.tree;
import gubg.coordinate;
import gubg.drawable;
import gubg.style;
import gubg.canvas;
import gubg.puts;

class Scene
{
    this(Canvas canvas)
	{
	    if (!canvas.initialize())
		err("Failed to initialize the canvas");
	    else
	    {
		mCanvas = canvas;
		auto transfo = new Transformation([0.5*canvas.width(),0.5*canvas.height()], [canvas.width()-1, canvas.height()-1]);
		mCurrentCoSystem = new CoSystem(transfo);
		mCurrentStyle = Style.defaultStyle();
		mTree = new Tree!(CoSystem)(mCurrentCoSystem);
	    }
	}
    bool draw()
	{
	    void drawDrawable(Drawable drawable)
	    {
		synchronized(drawable)
		{
		    drawable.draw(mCanvas, mCurrentCoSystem.transformation());
		}
	    }
	    if (mCanvas.initializeDraw())
	    {
		mCurrentCoSystem.eachDrawable(&drawDrawable);
		mCanvas.finalizeDraw();
	    } else
	    {
		err("Failed to initialize the canvas for drawing");
	    }
	    return true;
	}
    bool add(Drawable drawable, Style style = null)
	{
	    return mCurrentCoSystem.addDrawable(drawable, (style is null ? mCurrentStyle.dup() : style));
	}
    Style currentStyle(){return mCurrentStyle;}

    class CoSystem
    {
	this(Transformation transfo){mTransformation = transfo;}
	Transformation transformation(){return mTransformation;}
	bool addDrawable(Drawable drawable, Style style)
	    {
		drawable.setStyle(style);
		mDrawables ~= [drawable];
		return true;
	    }
	void eachDrawable(void delegate(Drawable drawable) callback)
	    {
		foreach(drawable; mDrawables)
		{
		    callback(drawable);
		}
	    }
    private:
	Transformation mTransformation;
	Drawable[] mDrawables;
    }
private:
    Canvas mCanvas;
    CoSystem mCurrentCoSystem;
    Style mCurrentStyle;
    Tree!(CoSystem) mTree;
}

version(Test)
{
    import tango.core.Thread;

    void main()
    {
//	Canvas canvas = new ConsoleCanvas;
	Canvas canvas = new SDLCanvas(640, 480);
	Scene scene = new Scene(canvas);
	scene.add(new Line([1,2], [3,4]));
	scene.add(new Line([10,20], [30,40]));
	auto circle = new Circle([12,34], 56);
	scene.add(circle);
	for (int i=0; i<100; ++i)
	{
	    scene.draw();
	    circle.setCenter([i,i]);
	    Thread.sleep(0.01);
	}
    }
}
