module vix.vix;

import vix.Exit;
import gubg.graphics.Canvas;
import gubg.graphics.IMUI;
import gubg.Format;
import gubg.OnlyOnce;
import gubg.FSTree;
import gubg.Layout;
import core.thread;

import gubg.Profiler;
import gubg.Timer;
import std.stdio;

const width = 1912;
const height = 1050;
int main(string[] args)
{
    auto widgets = new Widgets;
    auto canvas = new SDLCanvas(width, height);
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
    currentFolder.expand(creator, ExpandStrat.Shallow);

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

            auto box = new Box(TwoPoint(0, 0, width, height));
            const MaxNrEntries = 40;
            box.split(MaxNrEntries, Direction.TopDown);
            foreach (uint ix, ref sb; box)
            {
                if (ix >= currentFolder.childs.length+1)
                    break;
                FSTree child;
                string label;
                if (0 == ix)
                {
                    child = currentFolder.parent;
                    label = "..";
                }
                else
                {
                    child = currentFolder.childs[ix-1];
                    label = child.path;
                }
                auto w = widgets.get(ix);
                switch (w.process)
                {
                    case WidgetState.Empty:
                        w.set(new Button(sb.area, label, Alignment.Left, canvas));
                        break;
                    case WidgetState.Activated:
                        Folder folder = cast(gubg.FSTree.Folder)child;
                        if (folder)
                        {
                            currentFolder = folder;
                            currentFolder.expand(creator, ExpandStrat.Shallow);
                        }
                        break;
                    default:
                        w.get!(Button).setLabel(label);
                        break;
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
