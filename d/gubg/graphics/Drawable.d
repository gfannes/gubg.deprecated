module gubg.graphics.Drawable;

import gubg.Coordinate;

import gubg.graphics.Canvas;

interface IDrawable
{
    void draw(ICanvas, Transformation);
}

class StyledDrawable: IDrawable
{
    StrokeStyle strokeStyle(){return strokeStyle_;}
    FillStyle fillStyle(){return fillStyle_;}
    void strokeStyle(StrokeStyle style){strokeStyle_ = style;}
    void fillStyle(FillStyle style){fillStyle_ = style;}

    abstract void draw(ICanvas canvas, Transformation transfo);

    private:
    StrokeStyle strokeStyle_;
    FillStyle fillStyle_;
}

class Line: StyledDrawable
{
    this(real[] sco, real[] eco, StrokeStyle style)
    {
        origSCo_ = sco;
        origECo_ = eco;
        strokeStyle = style;
    }

    void setCoordinates(real[] sco, real[] eco){synchronized(this){origSCo_ = sco; origECo_ = eco;}}

    void draw(ICanvas canvas, Transformation transfo)
    {
        synchronized(this)
        {
            real[] sco, eco;
            transfo.transformCoN2O(sco, origSCo_);
            transfo.transformCoN2O(eco, origECo_);
            canvas.drawLine(sco, eco, strokeStyle);
        }
    }

    private:
    real[] origSCo_;
    real[] origECo_;
}
class Path: StyledDrawable
{
    this(real[] xcos, real[] ycos, StrokeStyle style)
    {
        origXCos_ = xcos;
        origYCos_ = ycos;
        strokeStyle = style;
    }

    void setCoordinates(real[] xcos, real[] ycos){synchronized(this){origXCos_ = xcos; origYCos_ = ycos;}}

    void draw(ICanvas canvas, Transformation transfo)
    {
        synchronized(this)
        {
            real[] sco, eco;
            foreach (ix, xco; origXCos_)
            {
                transfo.transformCoN2O(eco, [origXCos_[ix], origYCos_[ix]]);
                if (ix > 0)
                    canvas.drawLine(sco, eco, strokeStyle);
                sco = eco;
            }
        }
    }
 
    private:
    real[] origXCos_;
    real[] origYCos_;
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
            transfo.transformCoN2O(sco, origSCo_);
            transfo.transformCoN2O(eco, origECo_);
            canvas.drawRectangle(sco, eco, strokeStyle, fillStyle);
        }
    }
}

class Circle: StyledDrawable
{
    this(real[] center, real radius, StrokeStyle sStyle, FillStyle fStyle)
    {
        origCenter_ = center;
        origRadius_ = radius;
        strokeStyle = sStyle;
        fillStyle = fStyle;
    }

    void setCenter(real[] center){synchronized(this){origCenter_ = center;}}
    void setRadius(real radius){synchronized(this){origRadius_ = radius;}}

    void draw(ICanvas canvas, Transformation transfo)
    {
        synchronized(this)
        {
            real[] center, radiuss;
            transfo.transformCoN2O(center, origCenter_);
            transfo.transformLN2O(radiuss, [origRadius_, origRadius_]);
            canvas.drawCircle(center, 0.5*(radiuss[0]+radiuss[1]), strokeStyle, fillStyle);
        }
    }
    private:
    real[] origCenter_;
    real origRadius_;
}

version (UnitTest)
{
    void main()
    {

    }
}
