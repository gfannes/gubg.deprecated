#ifndef HEADER_gubg_visualization_canvas_hpp_ALREADY_INCLUDED
#define HEADER_gubg_visualization_canvas_hpp_ALREADY_INCLUDED

#include <SDL/SDL.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <iostream>

#include "basis.hpp"
#include "color.hpp"

using namespace std;

namespace gubg
{
    class Canvas
    {
    public:
        typedef enum
        {
            OX_Center,
            OX_Left,
            OX_Baseline,
            OX_Right,
        } OutlineX;
        typedef enum
        {
            OY_Center,
            OY_Top,
            OY_Baseline,
            OY_Bottom,
        } OutlineY;

        Canvas(int width, int height):
            mInitialized(false),
            mpSDLScreen(0),
            mpCairoSurface(0),
            mpCairoContext(0),
            mWidth(width),
            mHeight(height),
            mCurrentBasisID(-1),
            mFontSize(10.0)
            {
                setBasis(createBasis(0, 0, width, height));
            }
        ~Canvas(){SDL_Quit();};

        bool initialize()
            {
                if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
                {
                    cerr << "Could not initialize SDL" << endl;
                    return false;
                }
            
                mpSDLScreen = SDL_SetVideoMode( mWidth, mHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF ) ;
                if( mpSDLScreen == NULL )
                {
                    cerr << "Could not set the SDL video mode" << endl;
                    return false;
                }
                mpCairoSurface = Cairo::ImageSurface::create((unsigned char *)mpSDLScreen->pixels,Cairo::FORMAT_ARGB32,mWidth,mHeight,mWidth*4);
                mpCairoContext = Cairo::Context::create(mpCairoSurface);
                setFontSize(mFontSize);
                return true;
            }
        bool initialized(){return mInitialized;}

        long width(){return mWidth;}
        long height(){return mHeight;}

        void clear(unsigned int rgb = 0xff0000){SDL_FillRect( mpSDLScreen, NULL, rgb);}
        void flip(){SDL_Flip(mpSDLScreen);}

        void setColor(Color &color)
            {
                mpCairoContext->set_source_rgba(color.red,color.green,color.blue,color.alpha);
            }
        void setLineWidth(double lineWidth)
            {
                mpCairoContext->set_line_width(lineWidth);
            }
        void drawLine(double x1, double y1, double x2, double y2)
            {
                mCurrentBasis.virtual2Screen(x1,y1,x1,y1);
                mCurrentBasis.virtual2Screen(x2,y2,x2,y2);
//            cout << "Drawing line from (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << ")" << endl;
                mpCairoContext->move_to(x1,y1);
                mpCairoContext->line_to(x2,y2);
                mpCairoContext->stroke();
            }
        // An approximation using B-splines (See "Approximating a circle or an ellipse using four Bezier cubic splines")
        void drawCircle(double x, double y, double radius)
            {
                mCurrentBasis.virtual2Screen(x,y,x,y);
                double rw,rh,mw,mh;
                mCurrentBasis.virtualWH2ScreenWH(rw,rh,0.5*radius,0.5*radius);
                mw = 0.551784*rw;
                mh = 0.551784*rh;
                mpCairoContext->move_to(x+rw,y);
                mpCairoContext->curve_to(x+rw, y+mh, x+mw, y+rh, x,    y+rh);
                mpCairoContext->curve_to(x-mw, y+rh, x-rw, y+mh, x-rw, y);
                mpCairoContext->curve_to(x-rw, y-mh, x-mw, y-rh, x,    y-rh);
                mpCairoContext->curve_to(x+mw, y-rh, x+rw, y-mh, x+rw, y);
                mpCairoContext->stroke();
            }
        void fillCircle(double x, double y, double radius)
            {
                mCurrentBasis.virtual2Screen(x,y,x,y);
                double rw,rh,mw,mh;
                mCurrentBasis.virtualWH2ScreenWH(rw,rh,0.5*radius,0.5*radius);
                mw = 0.551784*rw;
                mh = 0.551784*rh;
                mpCairoContext->move_to(x+rw,y);
                mpCairoContext->curve_to(x+rw, y+mh, x+mw, y+rh, x,    y+rh);
                mpCairoContext->curve_to(x-mw, y+rh, x-rw, y+mh, x-rw, y);
                mpCairoContext->curve_to(x-rw, y-mh, x-mw, y-rh, x,    y-rh);
                mpCairoContext->curve_to(x+mw, y-rh, x+rw, y-mh, x+rw, y);
                mpCairoContext->fill();
            }
        void drawRectangle(double x, double y, double w, double h)
            {
                mCurrentBasis.virtual2Screen(x,y,x,y);
                mCurrentBasis.virtualWH2ScreenWH(w,h,w,h);
                mpCairoContext->rectangle(x,y,w,-h);
                mpCairoContext->stroke();
            }
        void fillRectangle(double x, double y, double w, double h)
            {
                mCurrentBasis.virtual2Screen(x,y,x,y);
                mCurrentBasis.virtualWH2ScreenWH(w,h,w,h);
                mpCairoContext->rectangle(x,y,w,-h);
                mpCairoContext->fill();
            }
        void drawPixel(int x, int y, double red, double green, double blue)
            {
                Uint8 R = (Uint8)(red*256), G = (Uint8)(green*256), B = (Uint8)(blue*256);
                Uint32 color = SDL_MapRGB(mpSDLScreen->format, R, G, B);
            
                if ( SDL_MUSTLOCK(mpSDLScreen) ) {
                    if ( SDL_LockSurface(mpSDLScreen) < 0 ) {
                        return;
                    }
                }
                switch (mpSDLScreen->format->BytesPerPixel) {
                case 1: { /* Assuming 8-bpp */
                    Uint8 *bufp;
                
                    bufp = (Uint8 *)mpSDLScreen->pixels + y*mpSDLScreen->pitch + x;
                    *bufp = color;
                }
                    break;
                
                case 2: { /* Probably 15-bpp or 16-bpp */
                    Uint16 *bufp;
                
                    bufp = (Uint16 *)mpSDLScreen->pixels + y*mpSDLScreen->pitch/2 + x;
                    *bufp = color;
                }
                    break;
                
                case 3: { /* Slow 24-bpp mode, usually not used */
                    Uint8 *bufp;
                
                    bufp = (Uint8 *)mpSDLScreen->pixels + y*mpSDLScreen->pitch + x;
                    *(bufp+mpSDLScreen->format->Rshift/8) = R;
                    *(bufp+mpSDLScreen->format->Gshift/8) = G;
                    *(bufp+mpSDLScreen->format->Bshift/8) = B;
                }
                    break;
                
                case 4: { /* Probably 32-bpp */
                    Uint32 *bufp;
                
                    bufp = (Uint32 *)mpSDLScreen->pixels + y*mpSDLScreen->pitch/4 + x;
                    *bufp = color;
                }
                    break;
                }
                if ( SDL_MUSTLOCK(mpSDLScreen) ) {
                    SDL_UnlockSurface(mpSDLScreen);
                }
            }

        bool setFontSize(double fontSize)
            {
                mFontSize = fontSize;
                if (!mpCairoContext)
                {
                    cerr << "Cairo context is not initialized in setFontSize" << endl;
                    return false;
                }
                mpCairoContext->set_font_size(mFontSize);
                mpCairoContext->get_font_extents(mFontExtent);
                return true;
            }
        double getFontSize(){return mFontSize;}

        // (nextX, nextY) are the coordinates of the right extent of txt as it was printed
        // This means you can start from that coordinate using OX_Baseline and OY_Baseline and it will look ok
        // If oy was OY_Baseline, nextY will be the same as y was
        void drawText(double x, double y, OutlineX ox, OutlineY oy, string txt, double &nextX, double &nextY)
            {
                mCurrentBasis.virtual2Screen(x,y,x,y);
                switch (oy)
                {
                case OY_Center:
                    y += 0.5*(mFontExtent.ascent - mFontExtent.descent);
                    break;
                case OY_Top:
                    y += mFontExtent.ascent;
                    break;
                case OY_Baseline:
                    // Do nothing
                    break;
                case OY_Bottom:
                    y -= mFontExtent.descent;
                    break;
                default:
                    cerr << "Unknown X outline option " << ox << endl;
                    break;
                }

                Cairo::TextExtents te;
                mpCairoContext->get_text_extents(txt, te);
                switch (ox)
                {
                case OX_Center:
                    x -= te.x_bearing + 0.5*te.width;
                    break;
                case OX_Left:
                    x -= te.x_bearing;
                    break;
                case OX_Baseline:
                    // Do nothing
                    break;
                case OX_Right:
                    x -= te.x_bearing + te.width;
                    break;
                default:
                    cerr << "Unknown X outline option " << ox << endl;
                    break;
                }
                mCurrentBasis.screen2Virtual(nextX, nextY, x + te.x_advance, y + te.y_advance);
            
                mpCairoContext->move_to(x, y);
                mpCairoContext->text_path(txt.c_str());
                mpCairoContext->fill();
                mpCairoContext->stroke();
            }
    
        int createBasis(double xO, double yO, double xW, double yW)
            {
                Basis basis(xO,yO,xW,yW,mHeight);
                basis.print();
                mBasises.push_back(basis);
                return mBasises.size()-1;
            }

        bool setBasis(int basis)
            {
                if (basis < 0 || basis >= mBasises.size())
                {
                    cerr << "Unknown basis " << basis << "" << endl;
                    return false;
                }
                if (basis != mCurrentBasisID)
                {
                    mCurrentBasisID = basis;
                    mCurrentBasis = mBasises[basis];
                }
                return true;
            }
        void getScreenLocation(int &ix, int &iy,double x, double y)
            {
                mCurrentBasis.virtual2Screen(x,y,x,y);
                ix = (int)x;
                iy = (int)y;
            }
        void getVirtualLocation(double &dx, double &dy,int x, int y)
            {
                mCurrentBasis.screen2Virtual(dx,dy,(double)x,(double)y);
            }
    private:
        bool mInitialized;
        int mWidth;
        int mHeight;
        SDL_Surface *mpSDLScreen;
        Cairo::RefPtr<Cairo::ImageSurface> mpCairoSurface;
        Cairo::RefPtr<Cairo::Context> mpCairoContext;    
        vector<Basis> mBasises;
        Basis mCurrentBasis;
        int mCurrentBasisID;
        double mFontSize;
        Cairo::FontExtents mFontExtent;
    };
};

#endif
