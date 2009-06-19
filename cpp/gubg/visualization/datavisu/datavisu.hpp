#ifndef datavisu_h
#define datavisu_h

#include <vector>

#include "drawable.hpp"
#include "visualization.hpp"
#include "utils.hpp"
#include "vector.hpp"
#include "function.hpp"
#include "color.hpp"
#include "sleep.hpp"

using namespace std;

namespace gubg
{
    class DataVisu: public Visualization
    {
	typedef struct
	{
	    vector<double> xs;
	    vector<double> ys;
	} Lines;
	typedef struct
	{
	    vector<double> xs;
	    vector<double> ys;
	    vector <int> marks;
	} Marks;
	typedef struct
	{
	    double x;
	    double y;
	    double w;
	    double h;
	    Color color;
	} Rectangles;
	typedef struct
	{
	    double minX;
	    double minY;
	    double maxX;
	    double maxY;
	    Function<double,0,1> *pFunction;
	} Functions;
    public:
	DataVisu(int width, int height, bool bShow = true):
	    Visualization(width, height, bShow){};
    
	bool addLines(vector<double> &xs, vector<double> &ys)
	    {
		Lines lines;
		lines.xs = xs;
		lines.ys = ys;
		mLines.push_back(lines);
		return true;
	    }
	// uses 0,1,2,3,... as xs
	bool addLines(vector<double> &ys)
	    {
		Lines lines;
		Vector::setEqual(lines.xs,ys.size(),0.0,ys.size());
		lines.ys = ys;
		mLines.push_back(lines);
		return true;
	    }
	bool addLine(double xs, double ys, double xe, double ye)
	    {
		Lines lines;
		Vector::set(lines.xs, xs, xe);
		Vector::set(lines.ys, ys, ye);
		mLines.push_back(lines);
		return true;
	    }
	//
	bool addMarks(vector<double> &xs, vector<double> &ys, vector<int> &marks)
	    {
		Marks ms;
            
	    }

	bool addRectangle(double x, double y, double w, double h, Color color = Color::Black())
	    {
		Rectangles rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		rect.color = color;
		mRectangles.push_back(rect);
		return true;
	    }

	// minO and maxO determine the minimum output and maximum output values that should be mapped onto black respectively white
	bool add2DFunction(double minX, double minY, double maxX, double maxY, Function<double,0,1> &function)
	    {
		bool ok = true;
		vector<double> xs,ys;
		Vector::set(xs,minX,minX,maxX,maxX,minX);
		Vector::set(ys,minY,maxY,maxY,minY,minY);
		ok = addLines(xs,ys);
		Functions functions;
		functions.minX = minX;
		functions.minY = minY;
		functions.maxX = maxX;
		functions.maxY = maxY;
		functions.pFunction = &function;
		mFunctions.push_back(functions);
		return ok;
	    }
	bool addMatrix(vector<vector<double> > &matrix);

	bool redraw(bool bClear = true)
	    {
		double minX,maxX,minY,maxY;
		double zeroX,zeroY;
		double lineWidth;
		if (!computeMinMax(minX,maxX,minY,maxY))
		{
		    cerr << "Could not compute the minmax" << endl;
		    return false;
		}
//            lineWidth = 2.0*Utils::min(double(maxX-minX)/width(),double(maxY-minY)/height());
		lineWidth = 1.0;

		zeroX = (-double(minX)*(width()-1.0)/(maxX-minX));
		zeroY = (-double(minY)*(height()-1.0)/(maxY-minY));

		int basis;
		basis=createBasis(zeroX,zeroY,(width()-1.0)/double(maxX-minX),(height()-1.0)/double(maxY-minY));
		for (int i=0;i<mFunctions.size();i++)
		{
		    Functions *f = &mFunctions[i];
		    Surface *surface = new Surface(*f->pFunction,f->minX,f->minY,f->maxX-f->minX,f->maxY-f->minY);
		    addDrawable(basis,surface,false);
		}
		for (int i=0;i<mLines.size();i++)
		{
		    for (int j=1;j<mLines[i].xs.size();j++)
		    {
			Line *line = new Line(mLines[i].xs[j-1],mLines[i].ys[j-1],mLines[i].xs[j],mLines[i].ys[j],lineWidth);
			addDrawable(basis,line,false);
		    }
		}
		for (int i=0;i<mRectangles.size();i++)
		{
//                 Rectangle *rect = new Rectangle(mRectangles[i].x,mRectangles[i].y,mRectangles[i].w,mRectangles[i].h,lineWidth,mRectangles[i].color);
//                 addDrawable(basis,rect,false);
		}
		if (bClear)
		    clear();
		flip();
		return true;
	    }
	bool show(bool useThread = false)
	    {
		initialize();
		redraw();
		mFirst = true;
		if (useThread)
		    Thread::start();
		else
		    Visualization::start();
		return true;
	    }

	bool clear()
	    {
		mLines.resize(0);
		mRectangles.resize(0);
		mFunctions.resize(0);
		return Visualization::clear(true);
	    }

    private:
	virtual bool dynamics()
	    {
		if (!mFirst)
		    nanosleep(0, 100000);
		else
		    mFirst = false;
		return true;
	    }
	bool computeMinMax(double &minX, double &maxX, double &minY, double &maxY)
	    {
		bool bFirst = true;
		for (int i=0;i<mLines.size();i++)
		{
		    for (int j=0;j<mLines[i].xs.size();j++)
		    {
			double x = mLines[i].xs[j];
			double y = mLines[i].ys[j];
			if (bFirst)
			{
			    minX = maxX = x;
			    minY = maxY = y;
			    bFirst = false;
			}
			if (x < minX)
			    minX = x;
			else if (x > maxX)
			    maxX = x;
			if (y < minY)
			    minY = y;
			else if (y > maxY)
			    maxY = y;
		    }
		}
		for (int i=0;i<mFunctions.size();i++)
		{
		    Functions *f = &mFunctions[i];
		    if (bFirst)
		    {
			minX = f->minX;
			maxX = f->maxX;
			minY = f->minY;
			maxY = f->maxY;
			bFirst = false;
		    }
		    if (f->minX < minX)
			minX = f->minX;
		    if (f->maxX > maxX)
			maxX = f->maxX;
		    if (f->minY < minY)
			minY = f->minY;
		    if (f->maxY > maxY)
			maxY = f->maxY;
		}
		for (int i=0;i<mRectangles.size();i++)
		{
		    Rectangles *f = &mRectangles[i];
		    if (bFirst)
		    {
			minX = f->x;
			maxX = f->x + f->w;
			minY = f->y;
			maxY = f->y + f->h;
			bFirst = false;
		    }
		    if (f->x < minX)
			minX = f->x;
		    if (((f->x)+(f->w)) > maxX)
			maxX = ((f->x)+(f->w));
		    if (f->y < minY)
			minY = f->y;
		    if (((f->y)+(f->h)) > maxY)
			maxY = ((f->y)+(f->h));
		}
		cout << "minX = " << minX << ", maxX = " << maxX << ", minY = " << minY << ", maxY = " << maxY << "" << endl;
		return true;
	    }

	vector<Lines> mLines;
	vector<Rectangles> mRectangles;
	vector<Functions> mFunctions;
	bool mFirst;
    };
};

#endif
