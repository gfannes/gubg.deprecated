module gubg.graphics.Scene;

import tango.core.sync.Mutex;

import gubg.Tree;
import gubg.Coordinate;

public import gubg.graphics.Drawable;
public import gubg.graphics.Canvas;

import gubg.Puts;

class Scene
{
    this(ICanvas canvas, real[] newOrigin = null, real[] newOneOne = null)
	{
	    if (!canvas.initialize())
		err("Failed to initialize the canvas");
	    else
	    {
		_canvas = canvas;
		// If no origin is specified, we take the middle of the canvas
		if (newOrigin is null)
		    newOrigin = [0.5*canvas.width(),0.5*canvas.height()];
		// If no (1, 1) is specified, we take the top right corner
		if (newOneOne is null)
		    newOneOne = [canvas.width()-1, canvas.height()-1];
		auto transfo = new Transformation(newOrigin, newOneOne);
		_currentCoSystem = new CoSystem(transfo);
		_tree = new Tree!(CoSystem)(_currentCoSystem);
	    }
	}
    void draw()
	{
	    void drawDrawable(IDrawable drawable)
	    {
		synchronized(drawable)
		{
		    drawable.draw(_canvas, _currentCoSystem.transformation());
		}
	    }
	    if (!_canvas.initializeDraw())
		throw new Exception("Failed to initialize the canvas for drawing");

	    _currentCoSystem.eachDrawable(&drawDrawable);
	    _canvas.finalizeDraw();
	}
    void add(IDrawable drawable){_currentCoSystem.addDrawable(drawable);}

    class CoSystem
    {
	this(Transformation transfo){mTransformation = transfo;}
	Transformation transformation(){return mTransformation;}
	void addDrawable(IDrawable drawable){_drawables ~= [drawable];}
	void eachDrawable(void delegate(IDrawable drawable) callback)
	    {
		foreach(drawable; _drawables)
		{
		    callback(drawable);
		}
	    }
    private:
	Transformation mTransformation;
	IDrawable[] _drawables;
    }
private:
    ICanvas _canvas;
    CoSystem _currentCoSystem;
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
	scene.add(new Line([-1.0, 1], [1.0, -1], StrokeStyle.standard));
	scene.add(new Line([-1.0, -1], [1.0, 1], StrokeStyle.standard));
 	auto circle = new Circle([0.0, 0], 0.3, StrokeStyle.standard, FillStyle.standard);
 	scene.add(circle);
	const nrSteps = 100;
	for (int i=0; i<nrSteps; ++i)
	{
	    scene.draw();
	    circle.setCenter([cast(real)i/nrSteps,cast(real)i/nrSteps]);
	    Thread.sleep(0.01);
	}
    }
}
