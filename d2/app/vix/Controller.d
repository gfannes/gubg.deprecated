module vix.Controller;

import vix.View;
import vix.Model;

import gubg.OnlyOnce;
import gubg.graphics.Canvas;
import gubg.graphics.IMUI;

import std.stdio;

class Controller
{
    this(uint width, uint height)
    {
        quit_ = false;

        fastProcessing_ = FewTimes(10);

        canvas_ = new SDLCanvas(width, height);

        model_ = new Model;
        view_ = new View(model_, canvas_);
    }
    
    bool quit(){return quit_;}
    void process()
    {
        scope ds = canvas_.new DrawScope;
        //Check the keyboard
        {
            auto key = canvas_.imui.getLastKey();
            if (Key.None != key)
                model_.appendToCommand(key);

            if (model_.quit)
                quit_ = true;
        }

        //Process the display
        view_.process();

        {
            //Give vix a few frames to get things up and running, then switch to a lower framerate
            if (canvas_.imui.waitForInput((fastProcessing_.isActive() ? 0.01 : 5.0)))
                fastProcessing_.reset();
        }
    }

    private:
        bool quit_;
        SDLCanvas canvas_;
        FewTimes fastProcessing_;
        View view_;
        Model model_;
}
