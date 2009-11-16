module gubg.graphics.drawable;

import gubg.coordinate;

import gubg.graphics.canvas;

interface IDrawable
{
    void draw(ICanvas, Transformation);
}

class StyledDrawable: IDrawable
{
    StrokeStyle strokeStyle(){return _strokeStyle;}
    FillStyle fillStyle(){return _fillStyle;}
    void strokeStyle(StrokeStyle style){_strokeStyle = style;}
    void fillStyle(FillStyle style){_fillStyle = style;}

    abstract void draw(ICanvas canvas, Transformation transfo);

private:
    StrokeStyle _strokeStyle;
    FillStyle _fillStyle;
}

class Line: StyledDrawable
{
    this(real[] sco, real[] eco, StrokeStyle style)
    {
	_origSCo = sco;
	_origECo = eco;
	strokeStyle = style;
    }

    void setCoordinates(real[] sco, real[] eco){synchronized(this){_origSCo = sco; _origECo = eco;}}

    void draw(ICanvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] sco, eco;
	    transfo.transformCoN2O(sco, _origSCo);
	    transfo.transformCoN2O(eco, _origECo);
	    canvas.drawLine(sco, eco, strokeStyle);
	}
    }
private:
    real[] _origSCo;
    real[] _origECo;
}

class Rectangle: Line
{
    this(real[] lbco, real[] trco, StrokeStyle sStyle, FillStyle fStyle)
    {
	super(lbco, trco, sStyle);
	fillStyle = fStyle;
    }

    void draw(ICanvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] sco, eco;
	    transfo.transformCoN2O(sco, _origSCo);
	    transfo.transformCoN2O(eco, _origECo);
	    canvas.drawRectangle(sco, eco, strokeStyle, fillStyle);
	}
    }
}

class Circle: StyledDrawable
{
    this(real[] center, real radius, StrokeStyle sStyle, FillStyle fStyle)
    {
	_origCenter = center;
	_origRadius = radius;
	strokeStyle = sStyle;
	fillStyle = fStyle;
    }

    void setCenter(real[] center){synchronized(this){_origCenter = center;}}
    void setRadius(real radius){synchronized(this){_origRadius = radius;}}

    void draw(ICanvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] center, radiuss;
	    transfo.transformCoN2O(center, _origCenter);
	    transfo.transformLN2O(radiuss, [_origRadius, _origRadius]);
	    canvas.drawCircle(center, 0.5*(radiuss[0]+radiuss[1]), strokeStyle, fillStyle);
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
