#ifndef drawable_h
#define drawable_h

#include "canvas.hpp"
#include "function.hpp"

namespace gubg
{
    class Drawable
    {
    public:
	Drawable():mBasis(-1){}
	void setBasis(int basis){mBasis = basis;};
	int getBasis(){return mBasis;};
	bool draw(Canvas &canvas)
	    {
//            cout << "mBasis = " << mBasis << "" << endl;
		return canvas.setBasis(mBasis) && drawLocal(canvas);
	    }
	virtual bool drawLocal(Canvas &canvas) = 0;
    private:
	int mBasis;
    };

    class Line: public Drawable
    {
    public:
	Line(double x1,double y1, double x2, double y2, double width = 1, Color color = Color::Black()):
	    mX1(x1),
	    mY1(y1),
	    mX2(x2),
	    mY2(y2),
	    mWidth(width),
	    mColor(color){}
	virtual bool drawLocal(Canvas &canvas)
	    {
		if (mColor.valid())
		{
		    canvas.setColor(mColor);
		    canvas.setLineWidth(mWidth);
		    canvas.drawLine(mX1,mY1,mX2,mY2);
		}
		return true;
	    }
	void setCoordinates(double x1,double y1, double x2, double y2)
	    {
		mX1 = x1; mY1 = y1; mX2 = x2; mY2 = y2;
	    }
    private:
	double mX1;
	double mY1;
	double mX2;
	double mY2;
	double mWidth;
	Color mColor;
    };

    class Circle: public Drawable
    {
    public:
	Circle(double x, double y, double r,double width = 1, Color strokeColor = Color::Black(), Color fillColor = Color::Invalid()):
	    mX(x),
	    mY(y),
	    mR(r),
	    mWidth(width),
	    mStrokeColor(strokeColor),
	    mFillColor(fillColor){};
	virtual bool drawLocal(Canvas &canvas)
	    {
		if (mWidth>0 && mStrokeColor.valid())
		{
		    canvas.setColor(mStrokeColor);
		    canvas.setLineWidth(mWidth);
		    canvas.drawCircle(mX,mY,mR);
		}
		if (mFillColor.valid())
		{
		    canvas.setColor(mFillColor);
		    canvas.fillCircle(mX,mY,mR);
		}
		return true;
	    }
	void setCentre(double x, double y){mX=x;mY=y;}
	void setRadius(double r){mR=r;}
    private:
	double mX;
	double mY;
	double mR;
	double mWidth;
	Color mStrokeColor;
	Color mFillColor;
    };

    class Rectangle: public Drawable
    {
    public:
	Rectangle(double x, double y, double w, double h, double linewidth = 1, Color strokeColor = Color::Black(), Color fillColor = Color::Invalid()):
	    mX(x),
	    mY(y),
	    mW(w),
	    mH(h),
	    mWidth(linewidth),
	    mStrokeColor(strokeColor),
	    mFillColor(fillColor)
	    {};
	virtual bool drawLocal(Canvas &canvas)
	    {
		if (mWidth>0 && mStrokeColor.valid())
		{
		    canvas.setColor(mStrokeColor);
		    canvas.setLineWidth(mWidth);
		    canvas.drawRectangle(mX,mY,mW,mH);
		}
		if (mFillColor.valid())
		{
		    canvas.setColor(mStrokeColor);
		    canvas.fillRectangle(mX,mY,mW,mH);
		}
		return true;
	    }
    private:
	double mX;
	double mY;
	double mW;
	double mH;
	double mWidth;
	Color mStrokeColor;
	Color mFillColor;
    };

// A centered rectangle
    class CRectangle: public Rectangle
    {
    public:
	CRectangle(double x, double y, double w, double h, double linewidth = 1, Color strokeColor = Color::Black(), Color fillColor = Color::Invalid()):
	    Rectangle(x-0.5*w,y-0.5*h,w,h,linewidth,strokeColor,fillColor){};
    };

// Is drawn using a Function or a function pointer
    class Surface: public Drawable
    {
    public:
	Surface(const Function<double,0,1> &function,double x,double y,double w,double h):
	    mX(x),
	    mY(y),
	    mW(w),
	    mH(h),
	    mFunctionBW(0),
	    mFunctionColor(0),
	    mFunctionBWPtr(0),
	    mFunctionColorPtr(0){function.clone(mFunctionBW);}
	Surface(const Function<double,1,1> &function,double x,double y,double w,double h):
	    mX(x),
	    mY(y),
	    mW(w),
	    mH(h),
	    mFunctionBW(0),
	    mFunctionColor(0),
	    mFunctionBWPtr(0),
	    mFunctionColorPtr(0){function.clone(mFunctionColor);}
	Surface(double (*function)(double,double),double x,double y,double w,double h):
	    mX(x),
	    mY(y),
	    mW(w),
	    mH(h),
	    mFunctionBW(0),
	    mFunctionColor(0),
	    mFunctionBWPtr(function),
	    mFunctionColorPtr(0){}
	Surface(void (*function)(double &, double &, double &,double,double),double x,double y,double w,double h):
	    mX(x),
	    mY(y),
	    mW(w),
	    mH(h),
	    mFunctionBW(0),
	    mFunctionColor(0),
	    mFunctionBWPtr(0),
	    mFunctionColorPtr(function){}
    private:

	virtual bool drawLocal(Canvas &canvas)
	    {
		int xb,yb,xe,ye,xm = canvas.width(),ym = canvas.height();            
		canvas.getScreenLocation(xb,yb,mX,mY);
		canvas.getScreenLocation(xe,ye,mX+mW,mY+mH);
		xb = (xb<0 ? 0 : xb);
		ye = (ye<0 ? 0 : ye);
		double x,y,r,g,b;
		vector<double> inputs,*outputs;
		inputs.resize(2);
		for (int i=xb;i<=xe && i<xm;++i)
		{
		    for (int j=ye;j<=yb && j<ym;++j)
		    {
			canvas.getVirtualLocation(x,y,i,j);
			if (mFunctionBW)
			{
			    inputs[0] = x;
			    inputs[1] = y;
			    r = g = b = mFunctionBW->output(inputs);
			} else if (mFunctionColor)
			{
			    inputs[0] = x;
			    inputs[1] = y;
			    outputs = &mFunctionColor->output(inputs);
			    r = (*outputs)[0];
			    g = (*outputs)[1];
			    b = (*outputs)[2];
			} else if (mFunctionBWPtr)
			{
			    r = g = b = mFunctionBWPtr(x,y);
			} else if (mFunctionColorPtr)
			{
			    mFunctionColorPtr(r,g,b,x,y);
			} else
			{
			    cerr << "No function pointer given" << endl;
			}
			canvas.drawPixel(i,j,r,g,b);
		    }
		}
	    }

	Function<double,0,1> *mFunctionBW;
	Function<double,1,1> *mFunctionColor;
	double (*mFunctionBWPtr)(double,double);
	void (*mFunctionColorPtr)(double &, double &, double &,double,double);
	double mX;
	double mY;
	double mW;
	double mH;
    };
};

#endif
