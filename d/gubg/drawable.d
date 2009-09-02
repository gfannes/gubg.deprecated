module gubg.drawable;

import gubg.canvas;
import gubg.coordinate;
import gubg.style;

class Drawable
{
    Style style(){return _style;}
    void setStyle(Style style)
	{
	    _style = style;
	}
    abstract bool draw(Canvas canvas, Transformation transfo);
private:
    Style _style;
}

class Line: Drawable
{
    this(real[] sco, real[] eco)
    {
	_origSCo = sco;
	_origECo = eco;
    }

    void setCoordinates(real[] sco, real[] eco){synchronized(this){_origSCo = sco; _origECo = eco;}}

    bool draw(Canvas canvas, Transformation transfo)
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

    bool draw(Canvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] sco, eco;
	    transfo.transformCoN2O(sco, _origSCo);
	    transfo.transformCoN2O(eco, _origECo);
	    if (_style.fill())
	    {
		canvas.setFillStyle(_style);
		canvas.fillRectangle(sco, eco);
	    }
	    if (_style.stroke())
	    {
		canvas.setStrokeStyle(_style);
		canvas.drawRectangle(sco, eco);
	    }
	    return true;
	}
    }
}

class Circle: Drawable
{
    this(real[] center, real radius)
    {
	_origCenter = center;
	_origRadius = radius;
    }

    void setCenter(real[] center){synchronized(this){_origCenter = center;}}
    void setRadius(real radius){synchronized(this){_origRadius = radius;}}

    bool draw(Canvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] center, radiuss;
	    transfo.transformCoN2O(center, _origCenter);
	    transfo.transformLN2O(radiuss, [_origRadius, _origRadius]);
	    if (_style.fill())
	    {
		canvas.setFillStyle(_style);
		canvas.fillCircle(center, 0.5*(radiuss[0]+radiuss[1]));
	    }
	    if (_style.stroke())
	    {
		canvas.setStrokeStyle(_style);
		canvas.drawCircle(center, 0.5*(radiuss[0]+radiuss[1]));
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
