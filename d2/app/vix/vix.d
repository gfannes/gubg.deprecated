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
            writefln("createFolder: %s", path);
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
            box.split([0.05, 0.95], Direction.TopDown);
            //The folder bar
            {
                auto folderBar = box[0];
                auto w = widgets.get();
                switch (w.process)
                {
                    case WidgetState.Empty:
                        w.set(new Button(folderBar.area, currentFolder.path, Alignment.Center, canvas));
                        break;
                    case WidgetState.Activated:
                        //What to do here?
                        break;
                    default:
                        w.get!(Button).setLabel(currentFolder.path);
                        break;
                }
            }
            box[1].split([0.02, 0.98], Direction.LeftToRight);
            //The back button
            {
                auto back = box[1][0];
                auto w = widgets.get();
                switch (w.process)
                {
                    case WidgetState.Empty:
                        w.set(new Button(back.area, "", Alignment.Left, canvas));
                        break;
                    case WidgetState.Activated:
                        if (currentFolder.parent)
                            currentFolder = currentFolder.parent;
                        currentFolder.expand(creator, ExpandStrat.Shallow);
                        break;
                    default:
                        break;
                }
            }
            //The file and folder buttons
            {
                const MaxNrEntries = 40;
                auto buttons = box[1][1];
                buttons.split(MaxNrEntries, Direction.TopDown);
                foreach (uint ix, ref sb; buttons)
                {
                    if (ix >= currentFolder.childs.length)
                        break;
                    FSTree child = currentFolder.childs[ix];
                    string label = child.name;
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
