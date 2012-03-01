#include "visualization.hpp"
#include <iostream>
using namespace std;
using namespace gubg;

void Visualization::initialize()
{
    if (_show)
        if (!_canvas.initialized())
            _canvas.initialize();
}

void Visualization::start()
{
    if (_show)
    {
        initialize();

        updateTime(true);

        bool continueDynamics;
        continueDynamics = onStart();

        quit_ = false;
        while ( !quit_ && continueDynamics )
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
            quit_ = true ;
        if ( Event.type == SDL_KEYDOWN )
        {
            if ( Event.key.keysym.sym == SDLK_ESCAPE )
                quit_ = true ;
        }
    }
}

void Visualization::draw()
{
    Drawable *drawable;
    int currentBasis=-2;
    vector<Drawable*>::iterator iter;

    DrawablesPtr::Unlock unlock(drawables_);
    vector<Drawable*> &drawables = unlock->getCurrent();
    _canvas.clear(_backgroundColor);
    for (iter=drawables.begin();iter!=drawables.end();iter++)
    {
        (*iter)->draw(_canvas);
    }

    //     // Add the frames per second
    //     mpCairoContext->set_matrix(_FPSBasis);
    //     mpCairoContext->text_path("Oe jee");
    //     mpCairoContext->fill();
    //     mpCairoContext->stroke();

    _canvas.flip();
}
