module vix.vix;

import vix.Exit;
import gubg.graphics.Canvas;
import gubg.graphics.IMUI;
import gubg.Format;
import gubg.OnlyOnce;
import gubg.FSTree;
import gubg.Layout;
import core.thread;
import std.algorithm;
import std.path;

import gubg.Profiler;
import gubg.Timer;
import std.stdio;

const width = 1912;
const height = 1050;
//const width = 640;
//const height = 480;
bool verbose__ = false;
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
        Folder createFolderRecursive(string path)
        {
            //The top-level Folder that will be returned
            Folder res = new Folder(path);
            //We will gradually step down until we reached to root of the file system
            string dir = dirname(path);
            Folder tmp = res;
            while (dir != path)
            {
                    tmp.parent = new Folder(dir);
                    tmp = tmp.parent;
                    auto t = dir;
                    dir = dirname(dir);
                    path = t;
            }
            return res;
        }
        gubg.FSTree.File createFile(string path)
        {
            return new gubg.FSTree.File(path);
        }
    }
    auto creator = new Creator;
    Folder currentFolder = creator.createFolderRecursive("/home/gfannes");
    currentFolder.expand(creator, ExpandStrat.Shallow);
    int topIX = 0;
    string command = "Command bar";

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
                        command = "";
                auto key = canvas.imui.getLastKey();
                if (Key.None != key)
                {
                    command ~= convertToChar(key);
                }
                if (!command.empty && '\n' == command[$-1])
                {
                    switch (command)
                    {
                        case ":q\n": quit = true; break;
                        case "quit\n": quit = true; break;
                        default: break;
                    }
                }
            }

            auto box = new Box(TwoPoint(0, 0, width, height));
            box.split([0.05, 0.92, 0.03], Direction.TopDown);
            auto folderBar = box[0];
            auto center = box[1];
            auto commandBar = box[2];
            //The folder bar
            {
                auto w = widgets.get();
                switch (w.process)
                {
                    case WidgetState.Empty:
                        w.set(new Button(folderBar.area, currentFolder.path, Alignment.Left, canvas));
                        break;
                    case WidgetState.Activated:
                        //What to do here?
                        break;
                    default:
                        w.get!(Button).setLabel(currentFolder.path);
                        break;
                }
            }
            center.split([0.02, 0.97, 0.01], Direction.LeftToRight);
            auto back = center[0];
            auto buttons = center[1];
            auto scroller = center[2];
            //The back button
            {
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
                        topIX = 0;
                        break;
                    default:
                        break;
                }
            }
            //The file and folder buttons with scrollbar
            {
                //Filter and sort the childs
                FSTree[] allChilds;
                {
                    //Append only the non-hidden files and folders
                    foreach (child; currentFolder.childs)
                        if (child.name[0] != '.')
                            allChilds ~= child;
                    //Sort the childs using localCmp as criterion
                    bool localCmp(FSTree lhs, FSTree rhs)
                    {
                        //First Folders, then Files
                        if (cast(Folder)lhs && cast(gubg.FSTree.File)rhs)
                            return true;
                        if (cast(gubg.FSTree.File)lhs && cast(Folder)rhs)
                            return false;
                        //If the type results in a tie, sort alphabetically
                        return lhs.name < rhs.name;
                    }
                    sort!(localCmp)(allChilds);
                }
                const MaxNrEntries = 40;
                //Scrollbar
                {
                    auto w = widgets.get();
                    switch (w.process)
                    {
                        case WidgetState.Empty:
                            w.set(new Scroller(scroller.area, buttons.area, canvas));
                            break;
                        case WidgetState.ScrollDown:
                            topIX = min(topIX+10, allChilds.length-1);
                            break;
                        case WidgetState.ScrollUp:
                            topIX = max(topIX-10, 0);
                            break;
                        default:
                            auto sb = w.get!(Scroller);
                            sb.setRange(allChilds.length);
                            sb.setBar([cast(real)topIX, min(topIX+MaxNrEntries, allChilds.length)]);
                            break;
                    }
                }
                buttons.split(MaxNrEntries, Direction.TopDown);
                auto childs = allChilds[topIX .. $];
                foreach (uint ix, ref sb; buttons)
                {
                    if (ix >= childs.length)
                        break;
                    FSTree child = childs[ix];
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
            //The command bar
            {
                auto w = widgets.get();
                switch (w.process)
                {
                    case WidgetState.Empty:
                        w.set(new Button(commandBar.area, command, Alignment.Left, canvas));
                        break;
                    case WidgetState.Activated:
                        //What to do here?
                        break;
                    default:
                        w.get!(Button).setLabel(command);
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
            if (verbose__)
            {
                writeln(p.toString);
                writefln("FPS: %s", cast(real)nrFrames/NrSeconds);
            }
            timer.reset;
            p.reset();
            nrFrames = 0;
        }
    }
    return 0;
}
