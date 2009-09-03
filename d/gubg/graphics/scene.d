module gubg.graphics.scene;

import tango.core.sync.Mutex;

import gubg.tree;
import gubg.coordinate;

import gubg.graphics.drawable;
import gubg.graphics.style;
import gubg.graphics.canvas;

import gubg.puts;

class Scene
{
    this(ICanvas canvas)
	{
	    if (!canvas.initialize())
		err("Failed to initialize the canvas");
	    else
	    {
		_canvas = canvas;
		auto transfo = new Transformation([0.5*canvas.width(),0.5*canvas.height()], [canvas.width()-1, canvas.height()-1]);
		_currentCoSystem = new CoSystem(transfo);
		_currentStyle = Style.defaultStyle();
		_tree = new Tree!(CoSystem)(_currentCoSystem);
	    }
	}
    bool draw()
	{
	    void drawDrawable(Drawable drawable)
	    {
		synchronized(drawable)
		{
		    drawable.draw(_canvas, _currentCoSystem.transformation());
		}
	    }
	    if (_canvas.initializeDraw())
	    {
		_currentCoSystem.eachDrawable(&drawDrawable);
		_canvas.finalizeDraw();
	    } else
	    {
		err("Failed to initialize the canvas for drawing");
	    }
	    return true;
	}
    bool add(Drawable drawable, Style style = null)
	{
	    return _currentCoSystem.addDrawable(drawable, (style is null ? _currentStyle.dup() : style));
	}
    Style currentStyle(){return _currentStyle;}

    class CoSystem
    {
	this(Transformation transfo){mTransformation = transfo;}
	Transformation transformation(){return mTransformation;}
	bool addDrawable(Drawable drawable, Style style)
	    {
		drawable.setStyle(style);
		_drawables ~= [drawable];
		return true;
	    }
	void eachDrawable(void delegate(Drawable drawable) callback)
	    {
		foreach(drawable; _drawables)
		{
		    callback(drawable);
		}
	    }
    private:
	Transformation mTransformation;
	Drawable[] _drawables;
    }
private:
    ICanvas _canvas;
    CoSystem _currentCoSystem;
    Style _currentStyle;
    Tree!(CoSystem) _tree;
}

version(UnitTest)
{
    import tango.core.Thread;

    void main()
    {
//	ICanvas canvas = new ConsoleCanvas;
	ICanvas canvas = new SDLCanvas(640, 480);
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
