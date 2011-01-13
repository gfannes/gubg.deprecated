module vix.vix;

import vix.Exit;
import gubg.graphics.Canvas;
import gubg.graphics.IMUI;
import gubg.Format;
import gubg.OnlyOnce;
import gubg.FSTree;
import core.thread;

import gubg.Profiler;
import gubg.Timer;
import std.stdio;

int main(string[] args)
{
    auto widgets = new Widgets;
    auto canvas = new SDLCanvas(1024, 768);
    auto fastProcessing = FewTimes(10);

    class Creator: ICreator
    {
        Folder createFolder(string path)
        {
            return new Folder(path);
        }
        gubg.FSTree.File createFile(string path)
        {
            return new gubg.FSTree.File(path);
        }
    }
    auto creator = new Creator;
    Folder currentFolder = creator.createFolder("/");
    currentFolder.expand(creator);

    //Profiling variables
    auto p = new Profiler("Mainloop");
    uint nrFrames = 0;
    auto timer = Timer(ResetType.NoAuto);

    bool quit = false;
    while (!quit)
    {
        ++nrFrames;
        {
            scope task = p.new Task("Drawing");
            scope ds = canvas.new DrawScope;

            //Check the keyboard
            {
                if (canvas.imui.escapeIsPressed())
                    quit = true;
                auto key = canvas.imui.getLastKey();
                if (Key.None != key)
                {
                    writefln("Key: %s", convertToChar(key));
                }
            }

            const NrRows = 10;
            const NrCols = 40;
            foreach (row; 0 .. NrRows)
            {
                foreach (col; 0 .. NrCols)
                {
                    task.change("Getting widgets");
                    auto w = widgets.get(row*NrCols+col);
                    task.change("Process");
                    switch (w.process)
                    {
                        case WidgetState.Empty:
                            auto dimensions = TwoPoint.centered((cast(real)canvas.width/NrRows)*(0.5+row),
                                    (cast(real)canvas.height/NrCols)*(0.5+col),
                                    0.8*cast(real)canvas.width/NrRows,
                                    0.8*cast(real)canvas.height/NrCols);
                            w.set(new Button(dimensions, Format.immediate("agl|(%s, %s)", row, col), Alignment.Left, canvas));
                            break;
                        default:
                            break;
                    }
                    task.change("Drawing");
                }
            }
        }

        {
            scope task = p.new Task("Sleeping");
            //Give vix a few frames to get things up and running, then switch to a lower framerate
            if (canvas.imui.waitForInput((fastProcessing.isActive() ? 0.01 : 5.0)))
                fastProcessing.reset();
        }

        //Every now and then, we write out profiling information
        const NrSeconds = 1.0;
        if (timer.difference > NrSeconds)
        {
            writeln(p.toString);
            writefln("FPS: %s", cast(real)nrFrames/NrSeconds);
            timer.reset;
            p.reset();
            nrFrames = 0;
        }
    }
    return 0;
}
