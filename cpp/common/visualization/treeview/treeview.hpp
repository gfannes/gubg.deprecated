#ifndef treeview_hpp
#define treeview_hpp

#include <vector>

#include "canvas.hpp"

using namespace std;

namespace gubg
{
    class TreeView
    {
    public:
	TreeView(int width, int height, int nrLines, int indentWidth = 2):
	    mWidth(width),
	    mHeight(height),
	    mNrLines(nrLines),
	    mCanvas(width, height),
	    mIndentWidth(indentWidth),
	    mQuit(false)
	    {
		mCanvas.initialize();
		const double bf = 0.2; // Fraction of fontSize to use as border
		double fontSize = double(mHeight)/(2*bf+nrLines);
		mCanvas.setFontSize(fontSize);
		mCanvas.setBasis(mCanvas.createBasis(bf*fontSize, mHeight-fontSize-bf*fontSize, mWidth-2*bf*fontSize, -fontSize));
	    }

	bool quit(){return mQuit;}

	bool checkKeys(vector<SDL_keysym> &keys)
	    {
		bool newEvents = false;
		keys.resize(0);
		static SDL_Event event;
		while (SDL_PollEvent(&event))
		{
		    if (event.type == SDL_QUIT)
			mQuit = true;
		    if (event.type == SDL_KEYDOWN)
		    {
			keys.push_back(event.key.keysym);
			newEvents = true;
		    }                
		}
		return newEvents;
	    }

	// fit indicates on return if the line did not exceed the right border
	bool drawLine(int line, int indent, string txt, bool *fit = NULL)
	    {
		double nx, ny;
		string str;
		indent *= mIndentWidth;
		str.reserve(indent+txt.size());
		str.resize(indent, ' ');
		str += txt;
            
		mCanvas.drawText(0, line, Canvas::OX_Baseline, Canvas::OY_Bottom, str, nx, ny);
		if (fit)
		    *fit = (nx < 1.0);
		return true;
	    }

	bool show()
	    {
		mCanvas.flip();
		mCanvas.clear();
	    }

	bool getFormat()
	    {
	    }
    
	bool clear()
	    {
	    }

	bool getInput()
	    {
	    }
    
    private:
	int mWidth;
	int mHeight;
	int mNrLines;
	int mIndentWidth;
	Canvas mCanvas;
	bool mQuit;
    };
};

#endif
