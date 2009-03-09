#include <iostream>

#include "visualization.hpp"

using namespace std;

void Visualization::initialize()
{
  if (mShow)
    if (!mCanvas.initialized())
      mCanvas.initialize();
}

void Visualization::start()
{
  if (mShow)
    {
      initialize();

      updateTime(true);

      bool continueDynamics;
      continueDynamics = onStart();

      mQuit = false;
      while ( !mQuit && continueDynamics )
        {
          handleInput();

          updateTime();

          continueDynamics = dynamics();

          draw();
        }

      SDL_Quit() ;
    }
}

void Visualization::handleInput()
{
  static SDL_Event Event ;
  while ( SDL_PollEvent(&Event) )
    {
      if ( Event.type == SDL_QUIT )
        mQuit = true ;
      if ( Event.type == SDL_KEYDOWN )
        {
          if ( Event.key.keysym.sym == SDLK_ESCAPE )
            mQuit = true ;
        }
    }
}

void Visualization::draw()
{
  Drawable *drawable;
  int currentBasis=-2;
  vector<Drawable*>::iterator iter;

  mDrawablesM.lock();
  vector<Drawable*> &drawables = mDrawables.getCurrent();
  mCanvas.clear(mBackgroundColor);
  for (iter=drawables.begin();iter!=drawables.end();iter++)
    {
      (*iter)->draw(mCanvas);
    }

//     // Add the frames per second
//     mpCairoContext->set_matrix(mFPSBasis);
//     mpCairoContext->text_path("Oe jee");
//     mpCairoContext->fill();
//     mpCairoContext->stroke();

  mCanvas.flip();
  mDrawablesM.unlock();
}
