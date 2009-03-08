module gubg.drawable;

import gubg.canvas;
import gubg.coordinate;
import gubg.style;

class Drawable
{
    Style style(){return mStyle;}
    void setStyle(Style style)
	{
	    mStyle = style;
	}
    abstract bool draw(Canvas canvas, Transformation transfo);
private:
    Style mStyle;
}

class Line: Drawable
{
    this(real[] sco, real[] eco)
    {
	mOrigSCo = sco;
	mOrigECo = eco;
    }

    void setCoordinates(real[] sco, real[] eco){synchronized(this){mOrigSCo = sco; mOrigECo = eco;}}

    bool draw(Canvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] sco, eco;
	    transfo.transformCoN2O(sco, mOrigSCo);
	    transfo.transformCoN2O(eco, mOrigECo);
	    return canvas.setStrokeStyle(mStyle) && canvas.drawLine(sco, eco);
	}
    }
private:
    real[] mOrigSCo;
    real[] mOrigECo;
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
	    transfo.transformCoN2O(sco, mOrigSCo);
	    transfo.transformCoN2O(eco, mOrigECo);
	    if (mStyle.fill())
	    {
		canvas.setFillStyle(mStyle);
		canvas.fillRectangle(sco, eco);
	    }
	    if (mStyle.stroke())
	    {
		canvas.setStrokeStyle(mStyle);
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
	mOrigCenter = center;
	mOrigRadius = radius;
    }

    void setCenter(real[] center){synchronized(this){mOrigCenter = center;}}
    void setRadius(real radius){synchronized(this){mOrigRadius = radius;}}

    bool draw(Canvas canvas, Transformation transfo)
    {
	synchronized(this)
	{
	    real[] center, radiuss;
	    transfo.transformCoN2O(center, mOrigCenter);
	    transfo.transformLN2O(radiuss, [mOrigRadius, mOrigRadius]);
	    if (mStyle.fill())
	    {
		canvas.setFillStyle(mStyle);
		canvas.fillCircle(center, 0.5*(radiuss[0]+radiuss[1]));
	    }
	    if (mStyle.stroke())
	    {
		canvas.setStrokeStyle(mStyle);
		canvas.drawCircle(center, 0.5*(radiuss[0]+radiuss[1]));
	    }
	    return true;
	}
    }
private:
    real[] mOrigCenter;
    real mOrigRadius;
}

version (UnitTest)
{
    void main()
    {
	
    }
}