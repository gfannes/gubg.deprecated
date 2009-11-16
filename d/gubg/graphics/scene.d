module gubg.graphics.scene;

import tango.core.sync.Mutex;

import gubg.tree;
import gubg.coordinate;

import gubg.graphics.drawable;
import gubg.graphics.canvas;

import gubg.puts;

class Scene
{
    this(ICanvas canvas, real[] newOrig = null, real[] newOne = null)
	{
	    if (!canvas.initialize())
		err("Failed to initialize the canvas");
	    else
	    {
		_canvas = canvas;
		if (newOrig is null)
		    newOrig = [0.5*canvas.width(),0.5*canvas.height()];
		if (newOne is null)
		    newOne = [canvas.width()-1, canvas.height()-1];
		auto transfo = new Transformation(newOrig, newOne);
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
	ICanvas canvas = new ConsoleCanvas;
//	ICanvas canvas = new SDLCanvas(640, 480);
	Scene scene = new Scene(canvas);
	scene.add(new Line([1.0, 2], [3.0, 4]));
	scene.add(new Line([10.0, 20], [30.0, 40]));
	auto circle = new Circle([12.0, 34], 56);
	scene.add(circle);
	for (int i=0; i<100; ++i)
	{
	    scene.draw();
	    circle.setCenter([i,i]);
	    Thread.sleep(0.01);
	}
    }
}
