module gubg.graphics.drawable;

import gubg.coordinate;

import gubg.graphics.canvas;
import gubg.graphics.style;

interface IDrawable
{
	bool draw(ICanvas, Transformation);
}

class StyledDrawable: IDrawable
{
    Style style(){return _style;}
    void setStyle(Style style)
	{
	    _style = style;
	}
    abstract bool draw(ICanvas canvas, Transformation transfo);
private:
    Style _style;
}

class Line: StyledDrawable
{
    this(real[] sco, real[] eco)
    {
	_origSCo = sco;
	_origECo = eco;
    }

    void setCoordinates(real[] sco, real[] eco){synchronized(this){_origSCo = sco; _origECo = eco;}}

    bool draw(ICanvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] sco, eco;
	    transfo.transformCoN2O(sco, _origSCo);
	    transfo.transformCoN2O(eco, _origECo);
	    return canvas.setStrokeStyle(_style) && canvas.drawLine(sco, eco);
	}
    }
private:
    real[] _origSCo;
    real[] _origECo;
}

class Rectangle: Line
{
    this(real[] lbco, real[] trco)
    {
	super(lbco, trco);
    }

    bool draw(ICanvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] sco, eco;
	    transfo.transformCoN2O(sco, _origSCo);
	    transfo.transformCoN2O(eco, _origECo);
	    if (_style.fill())
	    {
		canvas.setFillStyle(_style);
		canvas.drawRectangle(sco, eco, true);
	    }
	    if (_style.stroke())
	    {
		canvas.setStrokeStyle(_style);
		canvas.drawRectangle(sco, eco, false);
	    }
	    return true;
	}
    }
}

class Circle: StyledDrawable
{
    this(real[] center, real radius)
    {
	_origCenter = center;
	_origRadius = radius;
    }

    void setCenter(real[] center){synchronized(this){_origCenter = center;}}
    void setRadius(real radius){synchronized(this){_origRadius = radius;}}

    bool draw(ICanvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] center, radiuss;
	    transfo.transformCoN2O(center, _origCenter);
	    transfo.transformLN2O(radiuss, [_origRadius, _origRadius]);
	    if (_style.fill())
	    {
		canvas.setFillStyle(_style);
		canvas.drawCircle(center, 0.5*(radiuss[0]+radiuss[1]),true);
	    }
	    if (_style.stroke())
	    {
		canvas.setStrokeStyle(_style);
		canvas.drawCircle(center, 0.5*(radiuss[0]+radiuss[1]), false);
	    }
	    return true;
	}
    }
private:
    real[] _origCenter;
    real _origRadius;
}

version (UnitTest)
{
    void main()
    {
	
    }
}
