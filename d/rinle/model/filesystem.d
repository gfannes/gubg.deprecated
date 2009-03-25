module rinle.model.filesystem;

import rinle.model.interfaces;

import tango.io.FileSystem;

class FSNode: ICompositeNode
{
    this(char[] name)
	{
	    mName = name;
	}

    

//     void render(Sink sink)
//     {
//  	if (show)
//  	{
// 	    Tag tag;
// 	    tag.node = this;
// 	    tag.color = (select ? 1 : 0);
// 	    auto h = sink.create(tag, mName);
// 	    h.newline;
// 	    foreach (ix, ch; childs)
// 		ch.render(h);
// 	}
//     }

//     char[] name(){return mName;}

//     char[] path()
//     {
//         char[] path;
// 	FSNode f = this;
//         while (f !is null)
//         {
// 	    if (path.length == 0)
// 		path = f.mName;
// 	    else
// 		path = f.mName ~ "/" ~ path;
//             f = cast(FSNode)f.parent;
//         }
//         return path;
//     }

private:
    char[] mName;
}

class Dir: FSNode
{
    this()
    {
        super(FileSystem.getDirectory);
    }
}
